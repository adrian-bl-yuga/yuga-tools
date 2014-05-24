/* functions */
static void xdie(char *e);
static void sysfs_write(char *path, int value);
static void zap_core();
static void enable_core();
static void wait_for_screen_on();
static int get_clearpad_fd();
static double sysfs_read(char *path);
static int get_avg_val(char *path, int samples);
static int get_avg_cpu_usage(int core);
static int confget_max_num_cores();
static int confget_min_num_cores();
static int confget_powersave_bias();
char *sysfs_path_curfreq(int core);
char *sysfs_path_online(int core);
char *sysfs_path_runqueue(int core);
char *sysfs_path_maxfreq(int core);

#define FQD_DEBUG 0
#define debug_print(fmt, ...) \
  do { if (FQD_DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)


/*
 * MACHINE SPECIFIC DEFINITIONS
*/

/* frequencies supported by the CPU */
#define NUM_FREQ 12
static int available_freq[NUM_FREQ] = { 384000, 486000, 594000, 702000, 810000, 918000, 1026000, 1134000, 1242000, 1350000, 1458000, 1512000 };

#define CORE_MAX_FREQ available_freq[NUM_FREQ-1]
#define CORE_MIN_FREQ available_freq[0]

#define INPUT_NAME_TOUCHSCREEN  "clearpad"
#define SYSFS_LM3533_BRIGHTNESS "/sys/devices/i2c-0/0-0036/leds/lm3533-lcd-bl/brightness"
#define SYSFS_POWERSAVE_BIAS    "/sys/devices/system/cpu/cpufreq/ondemand/powersave_bias"

#define SETTINGS_BIAS_VALUE      "/data/misc/.pabx_settings_powersave_bias"
#define SETTINGS_MAX_NUM_CORES   "/data/misc/.pabx_settings_max_cores"
#define SETTINGS_MIN_NUM_CORES   "/data/misc/.pabx_settings_min_cores"

/* how many cores the system has */
#define NUM_CORES 4

#define MP_RAMP_UP 85 /* only go up if our average load is at least 85% */
#define MP_RAMP_DN 20 /* only go down if our load is <= 20%             */
#define MP_RAMP_VO  3 /* how many measurements to use                   */
#define MP_RUNQ_MIN 1.3 /* only bring new cores up if the worst run-queue is >= 1.3 */

#define PSB_TRIGGER 2

#define GLOBAL_SAMPLE_TARGET 800000 /* 0.8 second */
#define CORE_SAMPLE_TARGET    80000
