#include "public/tier0/utlhashmaplarge.h"

uint32 MurmurHash3ConstCharPtr::operator()( const char *pszKey ) const
{ return MurmurHash3String( pszKey ); }

bool CaseSensitiveStrEquals::operator()( const char *pszLhs, const char *pszRhs ) const
{ return strcmp( pszLhs, pszRhs ) == 0; }
