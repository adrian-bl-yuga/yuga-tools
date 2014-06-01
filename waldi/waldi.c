#include <stdio.h>
#include <utils/Log.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>

#include "waldi.h"

#define LOG_TAG "waldi"

int main() {
	pid_t bad_pid = 0;
	unsigned long wasted[2];
	char sysbuff[1023];

	for(;;) {
		bad_pid = get_pid_of_com_google_process_location();
		if(bad_pid > 0) {
			wasted[0] = get_utime(bad_pid);
			sleep(SAMPLING_INTERVAL);
			wasted[1] = (get_utime(bad_pid) - wasted[0])/SAMPLING_INTERVAL;
			debug_print(">> %s did %lu jiffies per second\n", WALDI_SEARCH_PROCESS, wasted[1]);
			
			if(wasted[1] > KILL_CEILING) {
				ALOGI("google went bananas, killing process with PID %d", bad_pid);
				snprintf(sysbuff, sizeof(sysbuff), "/system/bin/am force-stop %s", WALDI_MAIN_PROCESS);
				system(sysbuff);
			}
		}
		sleep(IDLE_SLEEP);
	}

	return 0;
}

/**
 * Returns '1' if given PID is currently running
 * Returns '0' if not or '-1' on error
 */
unsigned long get_utime(pid_t pid) {
	char buffer[1023];
	unsigned long result = 0;
	int fd = -1;
	ssize_t space_count = 0;
	ssize_t start_at = 0;
	ssize_t len, pos;

	snprintf(buffer, 32, "/proc/%d/stat", pid);
	fd = open(buffer, O_RDONLY, 0);
	if(fd < 0)
		goto early_exit;
	len = read(fd, buffer, sizeof buffer-1);
	buffer[len] = 0;
	close(fd);

	// Search for the char following the 2nd space:
	// PID (procname) S
	for(pos=0;pos<len;pos++) {
		if(buffer[pos] == ' ') {
			space_count++;
			if(space_count == 13) {
				start_at = pos+1; // next char will be the start of the new token
			} else if(space_count == 14) {
				buffer[pos] = 0;
				result = atol(&buffer[start_at]);
				break;
			}
		}
	}

early_exit:
	return result;
}

/**
 * Returns the PID of 'WALDI_SEARCH_PROCESS'
 * zero if the process was not found
 */
pid_t get_pid_of_com_google_process_location() {
	pid_t found_pid = 0;
	pid_t tmp_pid = 0;

	DIR *procfs;
	char buffer[100];
	struct dirent *dirent;
	int fd;

	procfs = opendir("/proc");
	if(procfs == NULL) {
		ALOGE("Failed to open procfs!");
		goto early_exit;
	}
	
	while( (dirent = readdir(procfs)) ) {
		errno = 0;
		tmp_pid = strtol(dirent->d_name, NULL, 10);
		if(errno != 0 || tmp_pid < 1)
			continue; // skip non-numeric entries

		// assemble file name and read contents
		snprintf(buffer, 35, "/proc/%d/cmdline", tmp_pid);
		fd = open(buffer, O_RDONLY, 0);
		if(fd < 0)
			continue;
		read(fd, buffer, sizeof buffer-1);
		close(fd);

		if(memcmp(WALDI_SEARCH_PROCESS, buffer, strlen(WALDI_SEARCH_PROCESS)) == 0) {
			found_pid = tmp_pid;
			break;
		}
	}

cleanup:
	closedir(procfs);
early_exit:
	return found_pid;
}
