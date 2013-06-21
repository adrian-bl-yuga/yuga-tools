/* functions */
static void xdie(char *e);
static void sysfs_write(char *path, int value);
static void zap_core();
static void enable_core();
static int sysfs_read(char *path);
static int get_avg_val(char *path, int samples);
static int get_avg_cpu_usage(int core);
static int bval(int a, int b);
char *sysfs_path_utilization(int core);
char *sysfs_path_online(int core);
char *sysfs_path_runqueue(int core);
char *sysfs_path_maxfreq(int core);


/*
 * MACHINE SPECIFIC DEFINITIONS
*/

/* frequencies supported by the CPU */
#define NUM_FREQ 12
static int available_freq[NUM_FREQ] = { 384000, 486000, 594000, 702000, 810000, 918000, 1026000, 1134000, 1242000, 1350000, 1458000, 1512000 };

#define CORE_MAX_FREQ available_freq[NUM_FREQ-1]
#define CORE_MIN_FREQ available_freq[0]

#define SYSFS_LM3533_BRIGHTNESS "/sys/devices/i2c-0/0-0036/leds/lm3533-lcd-bl/brightness"

/* how many cores the system has */
#define NUM_CORES 4

#define MP_RAMP_UP 85 /* only go up if our average load is at least 85% */
#define MP_RAMP_DN 20 /* only go down if our load is <= 20%             */
#define MP_RAMP_VO  3 /* how many measurements to use                   */
#define MP_RUNQ_MIN 2 /* only bring new cores up if the worst run-queue is >= 2 */

#define GLOBAL_SAMPLE_TARGET 1000000 /* 1 second */
#define CORE_SAMPLE_TARGET 100000
#define OFF_SAMPLE_DELAY_SEC 4
