#ifndef AIL_ENVIRONMENT_HPP
#define AIL_ENVIRONMENT_HPP

#if defined(_MSC_VER) || defined(__WIN32__) || defined(_WIN32) || defined(WIN32)
#define AIL_WINDOWS
#endif

#elif defined(__i386__) || defined(__alpha__) \
   || defined(__ia64) || defined(__ia64__) \
   || defined(_M_IX86) || defined(_M_IA64) \
   || defined(_M_ALPHA) || defined(__amd64) \
   || defined(__amd64__) || defined(_M_AMD64) \
   || defined(__x86_64) || defined(__x86_64__) \
   || defined(_M_X64) || defined(__bfin__)
   
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

#endif
