#ifndef AIL_TYPES_HPP
#define AIL_TYPES_HPP

#include <vector>
#include <string>
#include <set>
#include <ail/environment.hpp>
#include <boost/cstdint.hpp>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned long long ullong;

typedef std::vector<std::string> string_vector;
typedef std::set<std::string> string_set;

#ifdef AIL_64
typedef boost::int64_t word;
typedef boost::uint64_t uword;
#else
typedef boost::int32_t word;
typedef boost::uint32_t uword;
#endif

#endif
