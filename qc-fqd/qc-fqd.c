#include <stdio.h>
#include <utils/Log.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include "qc-fqd.h"

#define LOG_TAG "qc-fqd"

/* remembered powersave bias status -> avoid re-reading the file */
static int psb_status = PSB_OFF+1;


/**
 * try to find an online core and bring it down
 * this will never touch core0
*/

char *sysfs_path_utilization(int core) {
	static char buf[255];
	snprintf(buf, sizeof(buf)-1, "/sys/devices/system/cpu/cpu%d/cpufreq/cpu_utilization", core);
	return buf;
}
char *sysfs_path_online(int core) {
	static char buf[255];
	snprintf(buf, sizeof(buf)-1, "/sys/devices/system/cpu/cpu%d/online", core);
	return buf;
}
char *sysfs_path_runqueue(int core) {
	static char buf[255];
	snprintf(buf, sizeof(buf)-1, "/sys/devices/system/cpu/cpu%d/rq-stats/run_queue_avg", core);
	return buf;
}
char *sysfs_path_maxfreq(int core) {
	static char buf[255];
	snprintf(buf, sizeof(buf)-1, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq", core);
	return buf;
}

int main() {
	
	int ccore, cusage, crqueue;
	int ss_cores, ss_usage, ss_rqworst;
	struct timeval start, end;
	useconds_t sample_delay;
	
	int votes = 0;
	int vote_up = 0;
	int vote_down = 0;
	
	ALOGI("starting up");
	for(;;) {
		
		gettimeofday(&start, NULL);
		
		/* collect global system load */
		ss_cores = ss_usage = ss_rqworst = 0;
		for(ccore=0;ccore<NUM_CORES;ccore++) {
			cusage = get_avg_cpu_usage(ccore);
			crqueue = sysfs_read( sysfs_path_runqueue(ccore) );
			ss_rqworst = bval(ss_rqworst, crqueue);
			
			if(cusage >= 0) {
				ss_cores++; /* this core is online */
				ss_usage += cusage;
			}
		}
		ss_usage = ss_usage / ss_cores;
		votes++;
		if(ss_usage >= MP_RAMP_UP && ss_rqworst >= MP_RUNQ_MIN) {
			vote_up++;
		} else if (ss_usage <= MP_RAMP_DN) {
			vote_down++;
		}
		
		/* check if we have to change our powersave bias */
		if(ss_rqworst > PSB_TRIGGER && psb_status != PSB_OFF) { /* Turn on if we have a long wait queue */
			psb_status = PSB_OFF;
			sysfs_write(SYSFS_POWERSAVE_BIAS, psb_status);
		}
		else if(ss_rqworst <= PSB_TRIGGER && psb_status != PSB_ON && ss_cores == 1 && ss_usage < MP_RAMP_UP) {
			psb_status = PSB_ON;
			sysfs_write(SYSFS_POWERSAVE_BIAS, psb_status);
		}
		
//		printf("sysload=%d, wrq=%d, cores=%d, vote_up=%d, vote_down=%d, votes=%d, psb=%d\n", ss_usage, ss_rqworst, ss_cores, vote_up, vote_down, votes, psb_status);
		
		if(votes == MP_RAMP_VO) {
			if(vote_up == MP_RAMP_VO && ss_cores < NUM_CORES)
				enable_core();
			if(vote_down == MP_RAMP_VO && ss_cores > 1)
				zap_core();
			votes = vote_up = vote_down = 0;
		}
		
		gettimeofday(&end, NULL);
		sample_delay = GLOBAL_SAMPLE_TARGET - ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		if(sample_delay < 0)
			sample_delay = 0;
		usleep(sample_delay);
		
		/* Check if only one core is running while the screen is
		 * off. If it is: We are pretty idle and will probably
		 * stay like this for some time -> increase the sampling delay */
		if(ss_cores == 1 && sysfs_read(SYSFS_LM3533_BRIGHTNESS) == 0) {
			sleep(OFF_SAMPLE_DELAY_SEC);
		}
	}
	
	return 0;
}

/************************************************************
 * Try to bring one core down                               *
*************************************************************/
static void zap_core() {
	int i = NUM_CORES - 1;
	for( ; i > 0 ; i--) {
		if(sysfs_read(sysfs_path_online(i)) == 1) {
			sysfs_write(sysfs_path_online(i), 0);
			printf("zap_core: core %d is now offline\n", i);
			break;
		}
	}
}

/************************************************************
 * Bring a new core up                                      *
*************************************************************/
static void enable_core() {
	int i;
	for(i=1 ; i<NUM_CORES; i++) {
		if(sysfs_read(sysfs_path_online(i)) == 0) {
			sysfs_write(sysfs_path_online(i), 1);
			printf("enable_core: core %d is now online\n", i);
			break;
		}
	}
}

/************************************************************
 * Returns the avg cpu usage, taking the current speed      *
 * into account for the % value                             *
*************************************************************/
static int get_avg_cpu_usage(int core) {
	int max_freq, avg_usage;
	float ratio;
	int real_usage = -1;
	
	max_freq  = sysfs_read( sysfs_path_maxfreq(core) );
	
	if(max_freq > 0) {
		ratio = (float)CORE_MAX_FREQ / max_freq;
		avg_usage = get_avg_val( sysfs_path_utilization(core), 4);
		real_usage = avg_usage * ratio;
	}
	
	return real_usage;
}

/************************************************************
 * Takes a few samples from $path and returns the average   *
*************************************************************/
static int get_avg_val(char *path, int samples) {
	int i, r = 1;
	
	for(i=1; i<=samples; i++) {
		r += sysfs_read(path);
		if(i!=samples)
			usleep(CORE_SAMPLE_TARGET);
	}
	r = r/samples;
	return r;
}

/************************************************************
 * Return bigger integer value                              *
*************************************************************/
static int bval(int a, int b) {
	return (a > b ? a : b );
}


/************************************************************
 * Logs a fatal error and exits                             *
*************************************************************/
static void xdie(char *e) {
	printf("FATAL ERROR: %s\n", e);
	ALOGE("FATAL ERROR: %s\n", e);
	exit(1);
}


/************************************************************
 * Read integer value from sysfs                            *
*************************************************************/
static int sysfs_read(char *path) {
	char buf[64];
	int rv = -1;
	int fd = open(path, O_RDONLY);
	
	if (fd >= 0) {
		if( read(fd, buf, sizeof(buf)) != -1 ) {
			rv = atoi(buf);
		}
		close(fd);
	}
	return rv;
}

/************************************************************
 * Write integer to sysfs file                              *
*************************************************************/
static void sysfs_write(char *path, int value) {
	char buf[64];
	int len;
	int fd = open(path, O_WRONLY);

	if (fd < 0) {
		strerror_r(errno, buf, sizeof(buf));
		ALOGE("Error opening %s: %s\n", path, buf);
		return;
	}

	snprintf(buf, sizeof(buf)-1, "%d", value);
	len = write(fd, buf, strlen(buf));
	if (len < 0) {
		strerror_r(errno, buf, sizeof(buf));
		ALOGE("Error writing to %s: %s\n", path, buf);
	}
	else {
		ALOGI("Wrote %s to %s\n", buf, path);
	}
	close(fd);
}
