/*
 * ** PUBLIC DOMAIN **
 *
 * This is a dummy stub library to get rid of libqc-opt.
 *
 * Known functions of libqc-opt:
 *   
 *   00001017 T _ZN4MaAc12getErrorNameEv
 *   0000101d T _ZN4MaAc13updateOffsetsEPi
 *   00001063 T _ZN4MaAc4initEi
 *   00001013 T _ZN4MaAc6statusEv
 *   00000ff9 T _ZN4MaAcC1Ei
 *   00000fb5 T _ZN4MaAcC1EiPKtib
 *   00000ff9 T _ZN4MaAcC2Ei
 *   00000fb5 T _ZN4MaAcC2EiPKtib
 *   00001005 T _ZN4MaAcD1Ev
 *   00001005 T _ZN4MaAcD2Ev
 *   0000164d T activity_trigger_browser
 *   000014e9 T activity_trigger_init
 *   00001649 T activity_trigger_resume
 *   00001575 T activity_trigger_start
 *   00001089 T libVersionQuery
 *   00001159 T libqc_opt_deinit
 *   00001159 T libqc_opt_init
 *   000016dd T mpctl_send
 *   00001193 T perf_cpu_setoptions
 *   000011b1 T perf_is_class01
 *   00001289 T perf_lock_acq  *STUB*
 *   000012ab T perf_lock_rel  *STUB*
 *   00001185 T perf_vote_increase_ondemand_sdf
 *   00001177 T perf_vote_lower_ondemand_sdf
 *   0000115b T perf_vote_turnoff_ondemand_io_busy
 *   00001169 T perf_vote_turnon_ondemand_io_busy
 *   00000fa9 T qc_dr
 *   00000fa5 T qc_sc
 *   0000106f T qc_u_digit
 */

#include <log/log.h>
#include "qc-opt.h"

int perf_lock_acq(int handle, int duration, int *list) {
	ALOGE("pabx lock_acq for handle %d called", handle);
	return 1; // handle, 0 on error
}

int perf_lock_rel(int handle) {
	ALOGE("pabx lock_rel for handle %d called", handle);
	return PL_REQUEST_SUCCEEDED;
}

int perf_cpu_setoptions(int rq_type, int rq_value) {
	ALOGE("pabx cpu_setoptions rq_type=%d, rq_value=%d called", rq_type, rq_value);
	return 1;
}
