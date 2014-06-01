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

	pid_t gpl_pid = 0;
	int i = 0;
	int catched = 0;

	for(;;) {
		gpl_pid = get_pid_of_com_google_process_location();

		if(gpl_pid > 0) {
			catched = i = 0;
			for(; i<10; i++) {
				if(is_running(gpl_pid) == 1)
					catched++;
				sleep(1);
			}
			debug_print("checks=%d, catched=%d\n", i, catched);
		}

		sleep(15);
	}


	return 0;
}

/**
 * Returns '1' if given PID is currently running
 * Returns '0' if not or '-1' on error
 */
int is_running(pid_t pid) {
	char buffer[100];
	int fd;
	int result = -1;
	int space_count = 0;
	ssize_t len, pos;

	snprintf(buffer, 32, "/proc/%d/stat", pid);
	fd = open(buffer, O_RDONLY, 0);
	if(fd < 0)
		goto early_exit;
	len = read(fd, buffer, sizeof buffer-1);
	close(fd);

	// Search for the char following the 2nd space:
	// PID (procname) S
	for(pos=0;pos<len;pos++) {
		if(space_count == 2) {
			result = (buffer[pos] == 'R' ? 1 : 0);
			break;
		}
		if(buffer[pos] == ' ')
			space_count++;
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
