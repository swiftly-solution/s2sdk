#include "public/resourcefile/resourcetype.h"

bool FixupResourceName( CBufferString &name )
{
    if ( name.Length() == 0 )
		return true;

	if ( V_IsAbsolutePath( name.Get() ) || name.Get()[0] == '/' )
	{
		Warning( "FixupResourceName: Illegal full path passed in (\"%s\")!\n", name.Get() );
		name.Clear();
		return false;
	}

	const char *pszExtension = V_GetFileExtension( name.Get() );
	if ( !pszExtension )
	{
		Warning( "FixupResourceName: Illegal path, missing extension passed in (\"%s\")!\n", name.Get() );
		name.Clear();
		return false;
	}

	const char *pszName = name.Get();
	int nLength = name.Length();

	CPathBufferString fixed;
	char *pszFixed = fixed.SetLength( nLength );
	int nFixedLength = 0;
	char chLast = '\0';
	bool bHasDotParts = false;

	for ( int i = 0; i < nLength; i++ )
	{
		char ch = pszName[i];
		if ( ch >= 'A' && ch <= 'Z' )
			ch += 'a' - 'A';

		if ( ch == '\\' || ch == '/' )
		{
			ch = '/';
			if ( chLast == '/' )
				continue;
		}
		else if ( ( ch == '.' || ch == ':' ) && &pszName[i] < pszExtension - 1 )
		{
			bHasDotParts = true;
		}

		pszFixed[nFixedLength++] = ch;
		chLast = ch;
	}

	pszFixed[nFixedLength] = '\0';

	if ( bHasDotParts )
	{
		V_RemoveDotSlashes( pszFixed, '/' );
		nFixedLength = (int)strlen( pszFixed );
	}

	name.Clear();
	name.Insert( 0, pszFixed, nFixedLength );
	return true;
}

bool FixupResourceName( ResourceType_t nType, CBufferString &name )
{
	if ( name.Length() == 0 )
		return true;

	if ( V_IsAbsolutePath( name.Get() ) || name.Get()[0] == '/' )
	{
		Warning( "FixupResourceName: Illegal full path passed in (\"%s\")!\n", name.Get() );
		name.Clear();
		return false;
	}

	char szTypeExtension[sizeof( ResourceType_t ) + 1] = {};
	memcpy( szTypeExtension, &nType, sizeof( ResourceType_t ) );

	const char *pszExtension = V_GetFileExtension( name.Get() );
	if ( !pszExtension )
	{
		name.SetExtension( szTypeExtension );
	}
	else if ( V_stricmp_fast( szTypeExtension, pszExtension ) != 0 )
	{
		Warning( "WARNING: Resource name \"%s\" has the incorrect extension \"%s\" for the specified resource type (expected \"%s\")!\n", name.Get(), pszExtension, szTypeExtension );
		name.Clear();
		return false;
	}

	name.FixupPathName( CORRECT_PATH_SEPARATOR );
	name.ToLowerFast( 0 );
	name.FixSlashes( '/' );
	return true;
}

ResourceType_t DeduceResourceTypeFromResourceName( const char *pszName )
{
	const char *pszExtension = pszName ? V_GetFileExtension( pszName ) : nullptr;
	if ( !pszExtension )
		return 0;

	ResourceType_t nType = 0;

	for ( int i = 0; pszExtension[i] && pszExtension[i] != '_'; i++ )
	{
		if ( i == sizeof( ResourceType_t ) )
			return 0;

		char ch = pszExtension[i];
		if ( ch >= 'A' && ch <= 'Z' )
			ch += 'a' - 'A';

		nType |= (ResourceType_t)(uint8_t)ch << ( i * 8 );
	}

	return nType;
}

void CResourceName::CopyFrom( const CResourceName &other )
{
	m_Name.Clear();
	m_Name.Insert( 0, other.GetName() );
	m_Id = other.m_Id;
	m_Type = other.m_Type;
}

bool CResourceName::InitAndForceType( const char *pszName, ResourceType_t nType )
{
	Clear();
	m_Name.Insert( 0, pszName );

	if ( IsEmpty() )
		return true;

	bool bFixed = nType ? FixupResourceName( nType, m_Name ) : FixupResourceName( m_Name );
	if ( !bFixed )
		return false;

	m_Id = ResourceId_t( MurmurHash64( m_Name.Get(), m_Name.Length(), 0xEDABCDEF ) );
	m_Type = DeduceResourceTypeFromResourceName( m_Name.Get() );
	return true;
}

ResourceId_t::ResourceId_t()
: m_Value( 0 )
{}

ResourceId_t::ResourceId_t( uint64 nValue )
: m_Value( nValue )
{}

CResourceName::CResourceName()
: m_Type( 0 )
{}

CResourceName::CResourceName( const char *pszName )
: CResourceName()
{ Init( pszName ); }

CResourceName::CResourceName( const char *pszName, ResourceType_t nType )
: CResourceName()
{ InitAndForceType( pszName, nType ); }

CResourceName::CResourceName( const CResourceName &other )
: CResourceName()
{ CopyFrom( other ); }

CResourceName & CResourceName::operator=( const CResourceName &other )
{
	if(this == &other)
		return *this;

	CopyFrom( other );
	return *this;
}

bool CResourceName::Init( const char *pszName )
{ return InitAndForceType( pszName, 0 ); }

void CResourceName::Clear()
{
	m_Name.Clear();
	m_Id = ResourceId_t();
	m_Type = 0;
}

bool CResourceName::IsEmpty() const
{ return m_Name.Length() == 0; }

const char * CResourceName::GetName() const
{ return m_Name.Get(); }

ResourceId_t CResourceName::GetId() const
{ return m_Id; }

ResourceType_t CResourceName::GetType() const
{ return m_Type; }
