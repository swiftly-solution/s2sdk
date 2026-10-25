#include "public/tier0/utlrbtree.h"

bool CaselessStringLessThanIgnoreSlashes( const char * const &lhs, const char * const &rhs )	
{ 
	const char *pa = lhs;
	const char *pb = rhs;
	while ( *pa && *pb )
	{
		char a = *pa;
		char b = *pb;
		
		// Check for dir slashes.
		if ( a == '/' || a == '\\' )
		{
			if ( b != '/' && b != '\\' )
				return ('/' < b);
		}
		else
		{
			if ( a >= 'a' && a <= 'z' )
				a = 'A' + (a - 'a');
			
			if ( b >= 'a' && b <= 'z' )
				b = 'A' + (b - 'a');
				
			if ( a > b )
				return false;
			else if ( a < b )
				return true;
		}
		++pa;
		++pb;
	}
	
	if ( *pa != *pb )
	{
		return ( !*pa );
	}

	return false;
}

bool StringLessThan( const char * const &lhs, const char * const &rhs)			
{
	if ( !lhs ) return false;
	if ( !rhs ) return true;
	return ( strcmp( lhs, rhs ) < 0 );  
}

bool CaselessStringLessThan( const char * const &lhs, const char * const &rhs )	
{ 
	if ( !lhs ) return false;
	if ( !rhs ) return true;
	return ( stricmp( lhs, rhs ) < 0 ); 
}

bool FastCaselessStringLessThan( const char * const &lhs, const char * const &rhs )	
{ 
	if ( !lhs ) return false;
	if ( !rhs ) return true;
	return ( V_stricmp_fast( lhs, rhs ) < 0 ); 
}