#pragma once

#if defined(_MSC_VER)

# if defined(BASEUTIL_EXPORTS)
#  define BASE_UTIL_EXPORT __declspec(dllexport)
# else
#  define BASE_UTIL_EXPORT __declspec(dllimport)
# endif

#elif defined(__GUNC__)
# if defined(BASEUTIL_EXPORTS)
#  define BASE_UTIL_EXPORT __attribute__((visibility("default")))
# else
#  define BASE_UTIL_EXPORT
# endif

#else
#define BASE_UTIL_EXPORT
#endif
