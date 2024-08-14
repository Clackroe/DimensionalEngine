#ifndef VX_DEFINES
#define VX_DEFINES

// Core Defines

//----TYPES----

typedef unsigned char u8;

typedef unsigned short u16;

typedef unsigned int u32;

typedef unsigned long long u64;

// Signed int types.

typedef signed char i8;

typedef signed short i16;

typedef signed int i32;

typedef signed long long i64;

// Floating point types

typedef float f32;

typedef double f64;

//----HELPFUL----

// Thanks TheCherno!
#define BIT(x) (1 << x)

#define VX_EXPAND_MACRO(x) x
#define VX_STRINGIFY_MACRO(x) #x

//----SYSTEM INFO----
#if defined(_WIN32) || defined(_WIN64)
#define VX_WINDOWS
#define VX_PLATFORM "WINDOWS"

#elif defined(__linux__)
#define VX_LINUX
#define VX_PLATFORM "LINUX"

#elif defined(__APPLE__) && defined(__MACH__)
#define VX_MACOS
#define VX_PLATFORM "MACOS"

#else
#error UNKNOWN_PLATFORM
#endif
#endif

//----CORE-APP ARCH HELP----
#if defined(VX_WINDOWS)
#ifdef MYLIB_BUILD
#define VXCORE_API __declspec(dllexport)
#else
#define VXCORE_API __declspec(dllimport)
#endif
#elif defined(VX_LINUX)
#ifdef MYLIB_BUILD
#define VXCORE_API __attribute__((visibility("default")))
#else
#define VXCORE_API
#endif
#else
#define VXCORE_API
#endif
