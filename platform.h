#ifdef __unix__
#include <unistd.h>
#define STEPSIZE sysconf(_SC_PAGESIZE)
#else
#define STEPSIZE 4096
#endif
