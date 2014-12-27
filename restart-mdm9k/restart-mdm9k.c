/**
 * (C) 2014 Adrian Ulrich
 *
 * Simple application to restart the mdm9k processor from userspace
 *
 * PUBLIC DOMAIN
 */

#include <stdlib.h>
#include <fcntl.h>
#include <cutils/properties.h>

#define LOG_TAG "restart-mdm9k"
#include <utils/Log.h>

int main() {
	int fd = 0;
	fd = open("/dev/mdm", O_RDONLY);

	if (fd >= 0) {
	
		ALOGI("Stoping kickstart\n");
		property_set("ctl.stop", "kickstart");

		ALOGI("Sending SHUTDOWN_CHARM to modem\n");
		ioctl(fd, 0x4004CC0A, 0x0);

		sleep(3);

		ALOGI("Sending WAKE_CHARM to modem\n");
		ioctl(fd, 0xcc01, 0x0);

		ALOGI("Starting kickstart\n");
		property_set("ctl.start", "kickstart");
		close(fd);
	}
	else {
		ALOGE("Failed to open /dev/mdm, returned: %d\n", fd);
	}

	return 0;
}

