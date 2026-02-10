#ifndef BASE_H
#define BASE_H

// == PLATFORMS ===================================================================================

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

// == INCLUDES ====================================================================================

// -- Standard Inclusions -----------------------------------------------------

#include <stdint.h> 
#include <stddef.h> // size_t
#include <stdbool.h> 
#include <wchar.h>

#include <string.h>	  // memset, memcpy, memmove, memcmp
#include <stdalign.h> // alignof, alignas

#include <assert.h>

// -- Platform-Specifc inclusions ---------------------------------------------

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

// == PRIMITIVES ==================================================================================

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  b8;
typedef uint16_t b16;
typedef uint32_t b32;
typedef uint64_t b64;
typedef unsigned char byte;

typedef float   f32;
typedef double  f64;

typedef size_t 	  usize;
typedef ptrdiff_t isize;
typedef uintptr_t uptr;
typedef intptr_t  iptr;

typedef wchar_t wchar;

#endif // BASE_H
