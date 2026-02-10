#ifndef STANDARD_INCLUDES_H
#define STANDARD_INCLUDES_H

#include "platform_definitions.h"

// Windows
#if PLATFORM_WINDOWS
	#define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

// Apple
#if PLATFORM_MAC
	#include <unistd.h>
#endif

// Linux
#if PLATFORM_LINUX
    #include <unistd.h>
#endif

#endif // STANDARD_INCLUDES_H
