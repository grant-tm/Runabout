#ifndef PLATFORM_DEFINITIONS_H
#define PLATFORM_DEFINITIONS_H

// Windows
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
#else
    #define PLATFORM_WINDOWS 0
#endif

// Apple
#if defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MAC 1
#else
    #define PLATFORM_MAC 0
#endif

// Linux
#if defined(__linux__)
    #define PLATFORM_LINUX 1
#else
    #define PLATFORM_LINUX 0
#endif

#if (PLATFORM_WINDOWS + PLATFORM_MAC + PLATFORM_LINUX) != 1
    #error "Unsupported or multiple platforms detected"
#endif

#endif // PLATFORM_DEFINITIONS_H
