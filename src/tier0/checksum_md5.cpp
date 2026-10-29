#include "public/tier0/checksum_md5.h"

void MD5Value_t:: Zero()
{ memset( bits, 0, sizeof( bits ) ); }

bool MD5Value_t::operator==( const MD5Value_t &src ) const
{ return memcmp( bits, src.bits, sizeof( bits ) ) == 0; }

bool MD5Value_t::operator!=( const MD5Value_t &src ) const
{ return !( *this == src ); }
