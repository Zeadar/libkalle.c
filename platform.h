	#ifdef __unix__
	#include <unistd.h>
	#define STEPSIZE (getpagesize())
	#else
	#define STEPSIZE 4096
	#endif

