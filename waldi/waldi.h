/* functions */

pid_t get_pid_of_com_google_process_location();
int is_running(pid_t pid);

#define WALDI_SEARCH_PROCESS "com.google.process.location"

#define WALDI_DEBUG 1
#define debug_print(fmt, ...) \
  do { if (WALDI_DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

