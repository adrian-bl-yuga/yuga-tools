/* functions */

pid_t get_pid_of_com_google_process_location();
unsigned long get_utime(pid_t pid);

#define WALDI_SEARCH_PROCESS "com.google.process.location" // process to search for
#define WALDI_MAIN_PROCESS "com.google.android.gms"        // process we are killing via AM

#define IDLE_SLEEP 20         // keep idle for X seconds
#define SAMPLING_INTERVAL 5   // wait X seconds between collecting a sample
#define KILL_CEILING 80       // kill if bad process did more than X jiffies


#define WALDI_DEBUG 1
#define debug_print(fmt, ...) \
  do { if (WALDI_DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

