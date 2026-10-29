#include "public/schemalib/schematype.h"

uint32_t CSchemaType_Hash( const char *pString, int len )
{
	return MurmurHash2( pString, len, 0xBAADFEED );
}

bool CSchemaType:: IsValid()
{ return false; }

const char * CSchemaType::ToString( CBufferString &buff, bool bDontClearBuff )
{ return ""; }

void CSchemaType:: SpewDescription( LoggingChannelID_t channelID, const char *pszName )
{}

bool CSchemaType:: GetSizeAndAlignment( int &nSize, uint8_t &nAlignment )
{ return false; }

bool CSchemaType:: CanReinterpretAs( const CSchemaType *pType )
{ return false; }

SchemaMetaInfoHandle_t<CSchemaType> CSchemaType:: GetInnerType()
{ return nullptr; }

SchemaMetaInfoHandle_t<CSchemaType> CSchemaType:: GetInnermostType()
{ return nullptr; }

bool CSchemaType:: IsA( const CSchemaType *pType )
{ return false; }

CSchemaType * CSchemaType::InternalMatchInnerAs( SchemaTypeCategory_t eTypeCategory, SchemaAtomicCategory_t eAtomicCategory )
{ return nullptr; }

bool CSchemaType:: IsTypeByType( const CSchemaType *pType )
{ return false; }

bool CSchemaType:: IsTypeByName( const char *szName )
{ return false; }

bool CSchemaType:: IsBuiltinType( SchemaBuiltinType_t eType )
{ return false; }

bool CSchemaType:: IsAtomicType( int nAtomicID )
{ return false; }

bool CSchemaType:: IsDeclaredClass( const char *szClassName )
{ return false; }

bool CSchemaType:: IsDeclaredEnum( const char *szEnumName )
{ return false; }

bool CSchemaType:: DependsOnlyOnUnresolvedOrGlobalTypes( ISchemaSystemTypeScope *pTypeScope )
{ return false; }
