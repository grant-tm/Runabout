#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdlib.h>

// -- Asserts -------------------------------------------------------------------------------------

#ifndef DEBUG
    #if defined(_DEBUG) || !defined(NDEBUG)
        #define DEBUG 1
    #else
        #define DEBUG 0
    #endif
#endif

#define Assert(condition) do { if (!(condition)) { exit(1); } } while (0)

#if DEBUG
    #define DebugAssert(condition) Assert(condition)
#else
    #define DebugAssert(condition) do { (void) sizeof(condition); } while (0)
#endif

// ------------------------------------------------------------------------------------------------

#define IgnoreUnusedVariable(x) (void)(x)

#endif // SYSTEM_H

