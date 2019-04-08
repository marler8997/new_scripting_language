#define errorf(fmt,...)      fprintf(stderr, "Error: " fmt "\n", ##__VA_ARGS__)
#define errorfErrno(fmt,...) fprintf(stderr, "Error: " fmt ": %s (errno=%d)\n", ##__VA_ARGS__, strerror(errno), errno)
