#pragma once

#if defined(_MSC_VER) || defined(__WIN32__) || defined(_WIN32) || defined(WIN32)
#define AIL_WINDOWS
#endif
   
#if \
	defined(AMD64) || \
	defined(__ia64) || \
	defined(__ia64__) || \
	defined(_M_IA64) || \
	defined(__amd64) || \
	defined(__amd64__) || \
	defined(_M_AMD64) || \
	defined(__x86_64) || \
	defined(__x86_64__) || \
	defined(_M_X64)
#define AIL_64
#endif
