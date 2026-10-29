#ifndef KEYVALUES3_H
#define KEYVALUES3_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "bufferstring.h"
#include "generichash.h"
#include "strtools.h"
#include "utlbuffer.h"
#include "utlhashtable.h"
#include "utlmap.h"
#include "utlstring.h"
#include "utlstringtoken.h"
#include "utlsymboltablelarge.h"
#include "public/mathlib/vector4d.h"
#include "color.h"
#include "bitvec.h"
#include "public/entity2/entityhandle.h"

#include <type_traits>

#include "memdbgon.h"

class KeyValues3;
class CKeyValues3Array;
class CKeyValues3Table;
class CKV3Arena;
struct KV1ToKV3Translation_t;
struct KV3ToKV1Translation_t;

/* 
	KeyValues3 is a data storage format. See https://developer.valvesoftware.com/wiki/KeyValues3
	Supports various specific data types targeted at the Source2.
	Each specific type corresponds to one of the basic types.

	There are 2 ways to create KeyValues3:

	1. Via CKV3Arena:
	- KV's, arrays and tables are stored in fixed memory blocks (clusters) and therefore memory is allocated only when clusters are created.
	- Supports metadata and some other things.

	2. Directly through the constructor.
*/

// Quick way to iterate across whole kv3, to access currently iterated kv3 use iter.Get()
// Mostly useful to iterate unnamed data, like arrays of primitives
#define FOR_EACH_KV3( kv, iter ) \
	for ( CKeyValues3Iterator iter( kv ); iter.IsValid(); iter.Advance() )

struct KV3ID_t
{
	const char* m_name;
	uint64		m_data1;
	uint64		m_data2;
};

// encodings
const KV3ID_t g_KV3Encoding_Text 		= { "text", 0x41C58A33E21C7F3Cull, 0xDAA323A6DA77799ull };
const KV3ID_t g_KV3Encoding_Binary 		= { "binary", 0x40C1F7D81B860500ull, 0x14E76782A47582ADull };
const KV3ID_t g_KV3Encoding_BinaryLZ4 	= { "binary_lz4", 0x4F5C63A16847348Aull, 0x19B1D96F805397A1ull };
const KV3ID_t g_KV3Encoding_BinaryZSTD 	= { "binary_zstd", 0x4305FEF06F620A00ull, 0x29DBB14623045FA3ull };
const KV3ID_t g_KV3Encoding_BinaryBC 	= { "binary_bc", 0x4F6C95BC95791A46ull, 0xD2DFB7A1BC050BA7ull };
const KV3ID_t g_KV3Encoding_BinaryAuto 	= { "binary_auto", 0x45836B856EB109E6ull, 0x8C06046E3A7012A3ull };

// formats
const KV3ID_t g_KV3Format_Generic = { "generic", 0x469806E97412167Cull, 0xE73790B53EE6F2AFull };

enum KV1TextEscapeBehavior_t
{
	KV1_NO_ESCAPE_SEQUENCES = 0,
	KV1_HAS_ESCAPE_SEQUENCES = 1,
};

// Only LoadKV3FromOldSchemaText reads these, which LoadKV3 calls for old schema text
enum KV3LoadTextFlags_t
{
	KV3_LOAD_TEXT_NONE = 0,
	KV3_LOAD_TEXT_OBJECT_REFERENCES_AS_STRINGS = (1 << 0), // keeps "&name" and NULL object references as strings instead of resolving them
};

enum KV3SaveTextFlags_t
{
	KV3_SAVE_TEXT_NONE = 0,
	KV3_SAVE_TEXT_TAGGED = (1 << 0), // adds subtype name before value
};

PLATFORM_OVERLOAD void DebugPrintKV3( const KeyValues3* kv );

// When using some LoadKV3/SaveKV3 functions, KV3ID_t structures must be filled in, which specify the format or encoding of the data.

PLATFORM_OVERLOAD bool LoadKV3( CKV3Arena* context, CUtlString* error, CUtlBuffer* input, const KV3ID_t& format, const char* kv_name, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3( KeyValues3* kv, CUtlString* error, CUtlBuffer* input, const KV3ID_t& format, const char* kv_name, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3( KeyValues3* kv, CUtlString* error, const char* input, const KV3ID_t& format, const char* kv_name, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromFile( CKV3Arena* context, CUtlString* error, const char* filename, const char* path, const KV3ID_t& format, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromFile( KeyValues3* kv, CUtlString* error, const char* filename, const char* path, const KV3ID_t& format, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromJSON( KeyValues3* kv, CUtlString* error, const char* input, const char* kv_name, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromJSONFile( KeyValues3* kv, CUtlString* error, const char* path, const char* filename, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromKV1File( KeyValues3* kv, CUtlString* error, const char* path, const char* filename, KV1TextEscapeBehavior_t esc_behavior, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromKV1Text( KeyValues3* kv, CUtlString* error, const char* input, KV1TextEscapeBehavior_t esc_behavior, const char* kv_name, bool bIgnoreConditionals, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromKV1Text_Translated( KeyValues3* kv, CUtlString* error, const char* input, KV1TextEscapeBehavior_t esc_behavior, const KV1ToKV3Translation_t* translation, int nTranslations, const char* kv_name, bool bIgnoreConditionals, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromKV3OrKV1( KeyValues3* kv, CUtlString* error, CUtlBuffer* input, const KV3ID_t& format, const char* kv_name, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromOldSchemaText( KeyValues3* kv, CUtlString* error, CUtlBuffer* input, const KV3ID_t& format, const char* kv_name, uint loadFlags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3Text_NoHeader( KeyValues3* kv, CUtlString* error, const char* input, const KV3ID_t& format, const char* kv_name, uint loadFlags = KV3_LOAD_TEXT_NONE );

PLATFORM_OVERLOAD bool SaveKV3( const KV3ID_t& encoding, const KV3ID_t& format, const KeyValues3* kv, CUtlString* error, CUtlBuffer* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3AsJSON( const KeyValues3* kv, CUtlString* error, CUtlBuffer* output );
PLATFORM_OVERLOAD bool SaveKV3AsJSON( const KeyValues3* kv, CUtlString* error, CUtlString* output );
PLATFORM_OVERLOAD bool SaveKV3AsKV1Text( const KeyValues3* kv, CUtlString* error, CUtlBuffer* output, KV1TextEscapeBehavior_t esc_behavior );
PLATFORM_OVERLOAD bool SaveKV3AsKV1Text_Translated( const KeyValues3* kv, CUtlString* error, CUtlBuffer* output, KV1TextEscapeBehavior_t esc_behavior, const KV3ToKV1Translation_t* translation, int nTranslations );
PLATFORM_OVERLOAD bool SaveKV3Text_NoHeader( const KeyValues3* kv, CUtlString* error, CBufferString* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3Text_NoHeader( const KeyValues3* kv, CUtlString* error, CUtlBuffer* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3Text_NoHeader( const KeyValues3* kv, CUtlString* error, CUtlString* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3Text_ToString( const KV3ID_t& format, const KeyValues3* kv, CUtlString* error, CBufferString* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3Text_ToString( const KV3ID_t& format, const KeyValues3* kv, CUtlString* error, CUtlString* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3ToFile( const KV3ID_t& encoding, const KV3ID_t& format, const KeyValues3* kv, CUtlString* error, const char* filename, const char* path, uint flags = KV3_SAVE_TEXT_NONE );

typedef int32 KV3MemberId_t;
#define KV3_INVALID_MEMBER ((KV3MemberId_t)-1)

// AMNOTE: These constants aren't actual constants, but rather calculated at compile time
// but the way they are calculated is unknown, previously it was using CUtlLeanVector min/max calculations
// but in here they seem to not match that behaviour.
enum
{
	ALLOC_KV3TABLE_MIN = 4,
	ALLOC_KV3TABLE_MAX = 0x6186154,

	ALLOC_KV3ARRAY_MIN = 4,
	ALLOC_KV3ARRAY_MAX = 0xFFFFF7F,

	ALLOC_CONTEXT_NODELIST_MIN = 32,
	ALLOC_CONTEXT_NODELIST_MAX = INT_MAX
};

enum
{
	KV3_ARRAY_MAX_FIXED_MEMBERS = 6,
	KV3_TABLE_MAX_FIXED_MEMBERS = 8,

	KV3_CONTEXT_SIZE = 4608,

	KV3_ARRAY_INIT_SIZE = 32,
	KV3_TABLE_INIT_SIZE = 64,

	KV3_CLUSTER_MAX_ELEMENTS = 253
};

enum KV3Type_t : uint8
{
	KV3_TYPE_INVALID = 0,
	KV3_TYPE_NULL,
	KV3_TYPE_BOOL,
	KV3_TYPE_INT,
	KV3_TYPE_UINT,
	KV3_TYPE_DOUBLE,
	KV3_TYPE_STRING,
	KV3_TYPE_BINARY_BLOB,
	KV3_TYPE_ARRAY,
	KV3_TYPE_TABLE,

	KV3_TYPE_COUNT,
};

enum KV3TypeOpt_t : uint8
{
	KV3_TYPEOPT_NONE = 0,
	
	KV3_TYPEOPT_STRING_SHORT,
	KV3_TYPEOPT_STRING_EXTERN,
	
	KV3_TYPEOPT_BINARY_BLOB_EXTERN,
	
	KV3_TYPEOPT_ARRAY_FLOAT32,
	KV3_TYPEOPT_ARRAY_FLOAT64,
	KV3_TYPEOPT_ARRAY_INT16,
	KV3_TYPEOPT_ARRAY_INT32,
	KV3_TYPEOPT_ARRAY_UINT8_SHORT,
	KV3_TYPEOPT_ARRAY_INT16_SHORT,

	KV3_TYPEOPT_STRING_SYMBOL,

	KV3_TYPEOPT_ARRAY_UINT32,
	KV3_TYPEOPT_ARRAY_UINT64,
};

enum KV3TypeEx_t : uint8
{
	KV3_TYPEEX_INVALID = 0,
	KV3_TYPEEX_NULL,
	KV3_TYPEEX_BOOL,
	KV3_TYPEEX_INT,
	KV3_TYPEEX_UINT,
	KV3_TYPEEX_DOUBLE,

	KV3_TYPEEX_STRING			= KV3_TYPE_STRING,
	KV3_TYPEEX_STRING_SHORT		= (KV3_TYPEEX_STRING|(KV3_TYPEOPT_STRING_SHORT << 4)),
	KV3_TYPEEX_STRING_EXTERN	= (KV3_TYPEEX_STRING|(KV3_TYPEOPT_STRING_EXTERN << 4)),
	// AMNOTE: The string is a symbol in the owning arena's symbol table
	KV3_TYPEEX_STRING_SYMBOL	= (KV3_TYPEEX_STRING|(KV3_TYPEOPT_STRING_SYMBOL << 4)),

	KV3_TYPEEX_BINARY_BLOB			= KV3_TYPE_BINARY_BLOB,
	KV3_TYPEEX_BINARY_BLOB_EXTERN	= (KV3_TYPEEX_BINARY_BLOB|(KV3_TYPEOPT_BINARY_BLOB_EXTERN << 4)),

	KV3_TYPEEX_ARRAY				= KV3_TYPE_ARRAY,
	KV3_TYPEEX_ARRAY_FLOAT32		= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_FLOAT32 << 4)),
	KV3_TYPEEX_ARRAY_FLOAT64		= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_FLOAT64 << 4)),
	KV3_TYPEEX_ARRAY_INT16			= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_INT16 << 4)),
	KV3_TYPEEX_ARRAY_INT32			= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_INT32 << 4)),
	KV3_TYPEEX_ARRAY_UINT8_SHORT	= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_UINT8_SHORT << 4)),
	KV3_TYPEEX_ARRAY_INT16_SHORT	= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_INT16_SHORT << 4)),
	KV3_TYPEEX_ARRAY_UINT32			= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_UINT32 << 4)),
	KV3_TYPEEX_ARRAY_UINT64			= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_UINT64 << 4)),

	KV3_TYPEEX_TABLE = KV3_TYPE_TABLE,
};

enum KV3SubType_t : uint8
{
	KV3_SUBTYPE_INVALID = 0,

	// string types
	KV3_SUBTYPE_RESOURCE,
	KV3_SUBTYPE_RESOURCE_NAME,
	KV3_SUBTYPE_PANORAMA,
	KV3_SUBTYPE_SOUNDEVENT,
	KV3_SUBTYPE_SUBCLASS, // table type
	KV3_SUBTYPE_ENTITY_NAME, // string type
	KV3_SUBTYPE_LOCALIZE,

	KV3_SUBTYPE_UNSPECIFIED,
	KV3_SUBTYPE_NULL,
	KV3_SUBTYPE_BINARY_BLOB,
	KV3_SUBTYPE_ARRAY,
	KV3_SUBTYPE_TABLE,
	KV3_SUBTYPE_BOOL8,
	KV3_SUBTYPE_CHAR8,
	KV3_SUBTYPE_UCHAR32,
	KV3_SUBTYPE_INT8,
	KV3_SUBTYPE_UINT8,
	KV3_SUBTYPE_INT16,
	KV3_SUBTYPE_UINT16,
	KV3_SUBTYPE_INT32,
	KV3_SUBTYPE_UINT32,
	KV3_SUBTYPE_INT64,
	KV3_SUBTYPE_UINT64,
	KV3_SUBTYPE_FLOAT32,
	KV3_SUBTYPE_FLOAT64,
	KV3_SUBTYPE_STRING,
	KV3_SUBTYPE_POINTER,
	KV3_SUBTYPE_COLOR32,

	// vector types
	KV3_SUBTYPE_VECTOR,
	KV3_SUBTYPE_VECTOR2D,
	KV3_SUBTYPE_VECTOR4D,
	KV3_SUBTYPE_ROTATION_VECTOR,
	KV3_SUBTYPE_QUATERNION,
	KV3_SUBTYPE_QANGLE,
	KV3_SUBTYPE_MATRIX3X4,
	KV3_SUBTYPE_TRANSFORM,

	KV3_SUBTYPE_STRING_TOKEN,
	KV3_SUBTYPE_EHANDLE,

	KV3_SUBTYPE_COUNT,
};

enum KV3ArrayAllocType_t
{
	KV3_ARRAY_ALLOC_EXTERN = 0,
	KV3_ARRAY_ALLOC_NORMAL = 1,
	KV3_ARRAY_ALLOC_EXTERN_FREE = 2,
};

enum KV3ToStringFlags_t
{
	KV3_TO_STRING_NONE = 0,
	KV3_TO_STRING_DONT_CLEAR_BUFF = (1 << 0),
	KV3_TO_STRING_DONT_APPEND_STRINGS = (1 << 1),
	KV3_TO_STRING_APPEND_ONLY_NUMERICS = (1 << 2),
	KV3_TO_STRING_RETURN_NON_NUMERICS = (1 << 3),
};

enum KV3MetaDataFlags_t
{
	KV3_METADATA_MULTILINE_STRING = (1 << 0),
	KV3_METADATA_SINGLE_QUOTED_STRING = (1 << 1),
};

enum KeyValues3Flag_t : uint8
{
	KEYVALUES3_FLAG_NONE = 0,
	KEYVALUES3_FLAG_RESOURCE_REFERENCE = (1 << 0),
	KEYVALUES3_FLAG_MULTILINE_STRING = (1 << 1),
	KEYVALUES3_FLAG_LAST_VALUE = (1 << 2)
};

namespace KV3Helpers
{
	template <typename T, typename... Ts>
	constexpr size_t PackAlignOf()
	{
		if constexpr (sizeof...(Ts) == 0)
			return alignof(T);
		else
			return (alignof(T) > PackAlignOf<Ts...>()) ? alignof(T) : PackAlignOf<Ts...>();
	}

	template <size_t ALIGN, typename... Ts>
	constexpr size_t PackSizeOf( int size )
	{
		return ((ALIGN_VALUE( size * sizeof( Ts ), ALIGN )) + ... + 0);
	}
}

struct KV3MetaData_t
{
	KV3MetaData_t();

	void Clear();

	void Purge();

	typedef CUtlOrderedMap<int, CBufferString> CommentsMap_t;

	int 			m_nLine;
	int 			m_nColumn;
	uint			m_nFlags;
	CUtlSymbolLarge m_sName;
	CommentsMap_t 	m_Comments;
};

struct KV3BinaryBlob_t
{
	size_t m_nSize;
	union
	{
		const byte*	m_pubData;
		byte		m_ubData[1];
	};
	bool m_bFreeMemory;
};

class CKV3MemberName
{
public:
	template <size_t N>
	inline CKV3MemberName( const char (&pszString)[N], UtlSymLargeId_t symid = UTL_INVAL_SYMBOL_LARGE ) : m_nHashCode( pszString ), m_SymId( symid ), m_pszString( pszString ) {}

	// AMNOTE: Template is required to enforce compiler to pick the correct overload when inlining
	// as otherwise non templated overload would always win the pick thus no const folding would happen
	template <typename T, std::enable_if_t<std::is_same_v<T, const char *>, int> = 0>
	inline CKV3MemberName( T pszString, UtlSymLargeId_t symid = UTL_INVAL_SYMBOL_LARGE ) : m_nHashCode( 0 ), m_SymId( symid ), m_pszString( "" )
	{
		if(!pszString || !pszString[0])
			return;

		m_nHashCode = CUtlStringToken( pszString );
		m_pszString = pszString;
	}

	CKV3MemberName();
	CKV3MemberName( CUtlStringToken nHashCode, const char* pszString = "", UtlSymLargeId_t symid = UTL_INVAL_SYMBOL_LARGE );

	unsigned int GetHashCode() const;
	const char* GetString() const;
	UtlSymLargeId_t GetSymId() const;

protected:
	CUtlStringToken m_nHashCode;
	UtlSymLargeId_t m_SymId;
	const char* m_pszString;
};

// A CKV3MemberName that holds a copy of its string. Copies don't keep the symbol id.
class CKV3MemberNameWithStorage : public CKV3MemberName
{
public:
	CKV3MemberNameWithStorage();
	CKV3MemberNameWithStorage( const char *pszString );
	CKV3MemberNameWithStorage( const CKV3MemberNameWithStorage &other );

	CKV3MemberNameWithStorage &operator=( const CKV3MemberNameWithStorage &other );

	void Set( const char *pszString );

private:
	CBufferStringN<32> m_Storage;
};

template<size_t SIZE, typename T>
class CKeyValues3ClusterImpl;

using CKeyValues3Cluster = CKeyValues3ClusterImpl<KV3_CLUSTER_MAX_ELEMENTS, KeyValues3>;
using CKeyValues3TableCluster = CKeyValues3ClusterImpl<KV3_TABLE_INIT_SIZE, CKeyValues3Table>;
using CKeyValues3ArrayCluster = CKeyValues3ClusterImpl<KV3_ARRAY_INIT_SIZE, CKeyValues3Array>;

class KeyValues3
{
public:
	KeyValues3( KV3TypeEx_t type = KV3_TYPEEX_NULL, KV3SubType_t subtype = KV3_SUBTYPE_UNSPECIFIED );
	KeyValues3( int cluster_elem, KV3TypeEx_t type, KV3SubType_t subtype );
	~KeyValues3();

	CKV3Arena* GetContext() const;
	KV3MetaData_t* GetMetaData( CKV3Arena** ppCtx = nullptr ) const;

	bool HasFlag( KeyValues3Flag_t flag ) const;
	bool HasAnyFlags() const;
	KeyValues3Flag_t GetAllFlags() const;
	void SetAllFlags( KeyValues3Flag_t flags );
	void SetFlag( KeyValues3Flag_t flag, bool state );

	KV3Type_t GetType() const;
	KV3TypeEx_t GetTypeEx() const;
	KV3SubType_t GetSubType() const;

	const char* GetTypeAsString() const;
	const char* GetSubTypeAsString() const;

	const char* ToString( CBufferString& buff, uint flags = KV3_TO_STRING_NONE ) const;

	void SetToNull();
	bool IsNull() const;
	bool IsArray() const;
	bool IsTable() const;

	bool GetBool( bool defaultValue = false ) const;
	int8_t GetChar( int8_t defaultValue = 0 ) const;
	uint32_t GetUChar32( uint32_t defaultValue = 0 ) const;
	int8_t GetInt8( int8_t defaultValue = 0 ) const;
	uint8_t GetUInt8( uint8_t defaultValue = 0 ) const;
	int16_t GetShort( int16_t defaultValue = 0 ) const;
	uint16_t GetUShort( uint16_t defaultValue = 0 ) const;
	int32_t GetInt( int32_t defaultValue = 0 ) const;
	uint32_t GetUInt( uint32_t defaultValue = 0 ) const;
	int64_t GetInt64( int64_t defaultValue = 0 ) const;
	uint64_t GetUInt64( uint64_t defaultValue = 0 ) const;
	float GetFloat( float defaultValue = 0.0f ) const;
	double GetDouble( double defaultValue = 0.0 ) const;

	void SetBool( bool value );
	void SetChar( int8_t value );
	void SetUChar32( uint32_t value );
	void SetInt8( int8_t value );
	void SetUInt8( uint8_t value );
	void SetShort( int16_t value );
	void SetUShort( uint16_t value );
	void SetInt( int32_t value );
	void SetUInt( uint32_t value );
	void SetInt64( int64_t value );
	void SetUInt64( uint64_t value );
	void SetFloat( float value );
	void SetDouble( double value );

	void* GetPointer( void *defaultValue = ( void* )0 ) const;
	void SetPointer( void* ptr );
	
	CUtlStringToken GetStringToken( CUtlStringToken defaultValue = CUtlStringToken() ) const;
	void SetStringToken( CUtlStringToken token );

	CEntityHandle GetEHandle( CEntityHandle defaultValue = CEntityHandle() ) const;
	void SetEHandle( CEntityHandle ehandle );

	const char* GetString( const char *defaultValue = "" ) const;
	void SetString( const char* pString, KV3SubType_t subtype = KV3_SUBTYPE_STRING );
	void SetStringExternal( const char* pString, KV3SubType_t subtype = KV3_SUBTYPE_STRING );
	
	const byte* GetBinaryBlob() const;
	int GetBinaryBlobSize() const;
	void SetToBinaryBlob( const byte* blob, int size );
	void SetToBinaryBlobExternal( const byte* blob, int size, bool free_mem );

	Color GetColor( const Color &defaultValue = Color( 0, 0, 0, 255 ) ) const;
	void SetColor( const Color &color );

	Vector GetVector( const Vector &defaultValue = Vector( 0.0f, 0.0f, 0.0f ) ) const;
	Vector2D GetVector2D( const Vector2D &defaultValue = Vector2D( 0.0f, 0.0f ) ) const;
	Vector4D GetVector4D( const Vector4D &defaultValue = Vector4D( 0.0f, 0.0f, 0.0f, 0.0f ) ) const;
	Quaternion GetQuaternion( const Quaternion &defaultValue = Quaternion( 0.0f, 0.0f, 0.0f, 0.0f ) ) const;
	QAngle GetQAngle( const QAngle &defaultValue = QAngle( 0.0f, 0.0f, 0.0f ) ) const;
	matrix3x4_t GetMatrix3x4( const matrix3x4_t &defaultValue = matrix3x4_t( Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ) ) ) const;

	void SetVector( const Vector &vec );
	void SetVector2D( const Vector2D &vec2d );
	void SetVector4D( const Vector4D &vec4d );
	void SetQuaternion( const Quaternion &quat );
	void SetQAngle( const QAngle &ang );
	void SetMatrix3x4( const matrix3x4_t &matrix );

	int GetArrayElementCount() const;
	void SetArrayElementCount( int count, KV3TypeEx_t type = KV3_TYPEEX_NULL, KV3SubType_t subtype = KV3_SUBTYPE_UNSPECIFIED );

	void SetToEmptyArray();
	KeyValues3** GetArrayBase();

	KeyValues3* GetArrayElement( int elem );
	const KeyValues3 *GetArrayElement( int elem ) const { return const_cast<KeyValues3 *>(this)->GetArrayElement( elem ); }

	KeyValues3* ArrayInsertElementBefore( int elem );
	KeyValues3* ArrayInsertElementAfter( int elem );
	KeyValues3* ArrayAddElementToTail();

	void ArraySwapItems( int idx1, int idx2 );

	void ArrayRemoveElements( int elem, int num );
	void ArrayRemoveElement( int elem );

	void SetToEmptyTable();
	int GetMemberCount() const;

	CKeyValues3Table *GetTable();
	CKeyValues3Table *GetTable() const { return const_cast<KeyValues3 *>(this)->GetTable(); };

	// AMNOTE: The engine's KV3 savers refuse a table that has invalid member names
	bool HasInvalidMemberNames() const;
	void SetHasInvalidMemberNames( bool state );

	KeyValues3* GetMember( KV3MemberId_t id );
	const KeyValues3* GetMember( KV3MemberId_t id ) const { return const_cast<KeyValues3*>(this)->GetMember( id ); }

	const char* GetMemberName( KV3MemberId_t id ) const;
	CKV3MemberName GetKV3MemberName( KV3MemberId_t id ) const;

	CUtlStringToken GetMemberHash( KV3MemberId_t id ) const;

	KeyValues3* FindMember( const CKV3MemberName &name, KeyValues3* defaultValue = nullptr );
	const KeyValues3 *FindMember( const CKV3MemberName &name, KeyValues3 *defaultValue = nullptr ) const { return const_cast<KeyValues3 *>(this)->FindMember( name, defaultValue ); };

	// AMNOTE: Checks the member at hint first and sets hint to the index after the member found,
	// except in large tables that find members through a hash of their names, which leave hint unchanged
	KeyValues3* FindMember( const CKV3MemberName &name, KV3MemberId_t &hint, KeyValues3* defaultValue = nullptr );
	const KeyValues3 *FindMember( const CKV3MemberName &name, KV3MemberId_t &hint, KeyValues3 *defaultValue = nullptr ) const { return const_cast<KeyValues3 *>(this)->FindMember( name, hint, defaultValue ); };
	KeyValues3* FindOrCreateMember( const CKV3MemberName &name, bool *pCreated = nullptr );

	bool RemoveMember( KV3MemberId_t id );
	bool RemoveMember( const KeyValues3* kv );
	bool RemoveMember( const CKV3MemberName &name );

	KeyValues3* RenameMember( const CKV3MemberName &name, const CKV3MemberName &newName );

	void OverlayKeysFrom( const KeyValues3 *src, bool bRecursive );

	bool GetMemberBool( const CKV3MemberName &name, bool defaultValue = false ) const;
	int8_t GetMemberChar( const CKV3MemberName &name, int8_t defaultValue = 0 ) const;
	uint32_t GetMemberUChar32( const CKV3MemberName &name, uint32_t defaultValue = 0 ) const;
	int8 GetMemberInt8( const CKV3MemberName &name, int8 defaultValue = 0 ) const;
	uint8 GetMemberUInt8( const CKV3MemberName &name, uint8 defaultValue = 0 ) const;
	int16 GetMemberShort( const CKV3MemberName &name, int16 defaultValue = 0 ) const;
	uint16 GetMemberUShort( const CKV3MemberName &name, uint16 defaultValue = 0 ) const;
	int32 GetMemberInt( const CKV3MemberName &name, int32 defaultValue = 0 ) const;
	uint32 GetMemberUInt( const CKV3MemberName &name, uint32 defaultValue = 0 ) const;
	int64 GetMemberInt64( const CKV3MemberName &name, int64 defaultValue = 0 ) const;
	uint64 GetMemberUInt64( const CKV3MemberName &name, uint64 defaultValue = 0 ) const;
	float32 GetMemberFloat( const CKV3MemberName &name, float32 defaultValue = 0.0f ) const;
	float64 GetMemberDouble( const CKV3MemberName &name, float64 defaultValue = 0.0 ) const;
	void *GetMemberPointer( const CKV3MemberName &name, void *defaultValue = (void *)0 ) const;
	CUtlStringToken GetMemberStringToken( const CKV3MemberName &name, CUtlStringToken defaultValue = CUtlStringToken() ) const;
	CEntityHandle GetMemberEHandle( const CKV3MemberName &name, CEntityHandle defaultValue = CEntityHandle() ) const;
	const char *GetMemberString( const CKV3MemberName &name, const char *defaultValue = "" ) const;
	Color GetMemberColor( const CKV3MemberName &name, const Color &defaultValue = Color( 0, 0, 0, 255 ) ) const;
	Vector GetMemberVector( const CKV3MemberName &name, const Vector &defaultValue = Vector( 0.0f, 0.0f, 0.0f ) ) const;
	Vector2D GetMemberVector2D( const CKV3MemberName &name, const Vector2D &defaultValue = Vector2D( 0.0f, 0.0f ) ) const;
	Vector4D GetMemberVector4D( const CKV3MemberName &name, const Vector4D &defaultValue = Vector4D( 0.0f, 0.0f, 0.0f, 0.0f ) ) const;
	Quaternion GetMemberQuaternion( const CKV3MemberName &name, const Quaternion &defaultValue = Quaternion( 0.0f, 0.0f, 0.0f, 0.0f ) ) const;
	QAngle GetMemberQAngle( const CKV3MemberName &name, const QAngle &defaultValue = QAngle( 0.0f, 0.0f, 0.0f ) ) const;
	matrix3x4_t GetMemberMatrix3x4( const CKV3MemberName &name, const matrix3x4_t &defaultValue = matrix3x4_t( Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ) ) ) const;

	void SetMemberToNull( const CKV3MemberName &name );
	void SetMemberToEmptyArray( const CKV3MemberName &name );
	void SetMemberToEmptyTable( const CKV3MemberName &name );
	void SetMemberToBinaryBlob( const CKV3MemberName &name, const byte *blob, int size );
	void SetMemberToBinaryBlobExternal( const CKV3MemberName &name, const byte *blob, int size, bool free_mem );
	void SetMemberToCopyOfValue( const CKV3MemberName &name, KeyValues3 *other );

	void SetMemberBool( const CKV3MemberName &name, bool value );
	void SetMemberChar( const CKV3MemberName &name, int8_t value );
	void SetMemberUChar32( const CKV3MemberName &name, uint32_t value );
	void SetMemberInt8( const CKV3MemberName &name, int8 value );
	void SetMemberUInt8( const CKV3MemberName &name, uint8 value );
	void SetMemberShort( const CKV3MemberName &name, int16 value );
	void SetMemberUShort( const CKV3MemberName &name, uint16 value );
	void SetMemberInt( const CKV3MemberName &name, int32 value );
	void SetMemberUInt( const CKV3MemberName &name, uint32 value );
	void SetMemberInt64( const CKV3MemberName &name, int64 value );
	void SetMemberUInt64( const CKV3MemberName &name, uint64 value );
	void SetMemberFloat( const CKV3MemberName &name, float32 value );
	void SetMemberDouble( const CKV3MemberName &name, float64 value );
	void SetMemberPointer( const CKV3MemberName &name, void *ptr );
	void SetMemberStringToken( const CKV3MemberName &name, CUtlStringToken token );
	void SetMemberEHandle( const CKV3MemberName &name, CEntityHandle ehandle );
	void SetMemberString( const CKV3MemberName &name, const char *pString, KV3SubType_t subtype = KV3_SUBTYPE_STRING );
	void SetMemberStringExternal( const CKV3MemberName &name, const char *pString, KV3SubType_t subtype = KV3_SUBTYPE_STRING );
	void SetMemberColor( const CKV3MemberName &name, const Color &color );
	void SetMemberVector( const CKV3MemberName &name, const Vector &vec );
	void SetMemberVector2D( const CKV3MemberName &name, const Vector2D &vec2d );
	void SetMemberVector4D( const CKV3MemberName &name, const  Vector4D &vec4d );
	void SetMemberQuaternion( const CKV3MemberName &name, const Quaternion &quat );
	void SetMemberQAngle( const CKV3MemberName &name, const QAngle &ang );
	void SetMemberMatrix3x4( const CKV3MemberName &name, const matrix3x4_t &matrix );

	KeyValues3& operator=( const KeyValues3& src );
	KeyValues3( const KeyValues3 &other ) : KeyValues3() { CopyFrom( &other ); }

	union Data_t
	{
		Data_t();

		bool	m_Bool;
		int64	m_Int;
		uint64	m_UInt;
		float64	m_Double;

		const char* m_pString;
		char m_szStringShort[8];
		UtlSymLargeId_t m_StringSymbol;

		KV3BinaryBlob_t* m_pBinaryBlob;

		CKeyValues3Array* m_pArray;
		CKeyValues3Table* m_pTable;

		union Array_t
		{
			float32* m_f32;
			Vector *m_vec;
			Vector2D *m_vec2;
			Vector4D *m_vec4;
			Quaternion *m_quat;
			QAngle *m_ang;
			matrix3x4_t *m_mat;
			float64* m_f64;
			int16* m_i16;
			int32* m_i32;
			uint32* m_u32;
			uint64* m_u64;
			uint8 m_u8Short[8];
			int16 m_i16Short[4];
		} m_Array;

		uint64 m_nMemory;
		void* m_pMemory;
		char m_Memory[1];
	};

private:
	void Alloc( int initial_size = 0, Data_t data = {}, int bytes_available = 0, bool should_free = false );

	CKeyValues3Array *AllocArray( int initial_size = 0 );
	CKeyValues3Table *AllocTable( int initial_size = 0 );

	void AllocArrayInPlace( int initial_size, Data_t data, int preallocated_size, bool should_free );
	void AllocTableInPlace( int initial_size, Data_t data, int preallocated_size, bool should_free );

	template <typename T>
	T *AllocateOnHeap( int initial_size = 0 );

	template <typename T>
	void FreeOnHeap( T *element );

	void FreeArray( CKeyValues3Array *element, bool clearing_context = false );
	void FreeTable( CKeyValues3Table *element, bool clearing_context = false );

	KeyValues3 *AllocMember( KV3TypeEx_t type = KV3_TYPEEX_NULL, KV3SubType_t subtype = KV3_SUBTYPE_UNSPECIFIED );
	void FreeMember( KeyValues3 *member );

	void Free( bool bClearingContext = false );
	void ResolveUnspecified();
	void PrepareForType( KV3TypeEx_t type, KV3SubType_t subtype, int initial_size = 0 );
	void CopyFrom( const KeyValues3* pSrc );

	int GetClusterElement() const;
	void SetClusterElement( int element );
	CKeyValues3Cluster* GetCluster() const;

	template < typename T > T FromString( T defaultValue ) const;
	template < typename T > void SetDirect( T value );

	template < typename T > T GetValue( T defaultValue ) const;
	template < typename T > void SetValue( T value, KV3TypeEx_t type, KV3SubType_t subtype );

	template < typename T > T GetVecBasedObj( int size, const T &defaultValue ) const;
	template < typename T > void SetVecBasedObj( const T &obj, int size, KV3SubType_t subtype );

	template < typename T >
	void NormalizeArray( KV3TypeEx_t type, KV3SubType_t subtype, int size, const T* data, bool bFree );
	void NormalizeArray();

	template < typename T >
	void AllocArray( int size, const T* data, KV3ArrayAllocType_t alloc_type, KV3TypeEx_t type_short, KV3TypeEx_t type_ptr, KV3SubType_t subtype, KV3TypeEx_t type_elem, KV3SubType_t subtype_elem );

	bool ReadArrayInt32( int size, int32* data ) const;
	bool ReadArrayFloat32( int size, float32* data ) const;

	static constexpr size_t TotalSizeOf( int initial_size ) { return sizeof(KeyValues3); }
	static constexpr size_t TotalSizeOfData( int size ) { return sizeof(Data_t); }
	static constexpr size_t TotalSizeWithoutStaticData() { return sizeof(KeyValues3) - TotalSizeOfData( 0 ); }

private:
	uint64 m_bContextIndependent : 1;
	uint64 m_bFreeArrayMemory : 1;
	uint64 m_TypeEx : 8;
	uint64 m_SubType : 8;
	uint64 m_nClusterElement : 16;
	uint64 m_nNumArrayElements : 5;
	uint64 m_nFlags : 8;
	uint64 m_nReserved : 17;
	Data_t m_Data;

	friend CKeyValues3Cluster;
	friend CKeyValues3ArrayCluster;
	friend CKeyValues3TableCluster;
	friend class CKV3Arena;
	friend class CKeyValues3Table;
	friend class CKeyValues3Array;
};
COMPILE_TIME_ASSERT(sizeof(KeyValues3) == 16);

class CKeyValues3Iterator
{
public:
	CKeyValues3Iterator();
	CKeyValues3Iterator( KeyValues3 *kv );

	void Init( KeyValues3 *kv );

	void Advance();

	KeyValues3 *Get() const;
	bool IsValid() const;

private:
	struct StackEntry_t
	{
		KeyValues3 *m_pKV;
		int m_nIndex;
	};

	CUtlVectorFixedGrowable<StackEntry_t, 4> m_Stack;
};

class CKeyValues3Array
{
public:
	typedef KeyValues3 *Element_t;

	static const size_t DATA_SIZE = KV3_ARRAY_MAX_FIXED_MEMBERS;
	static const size_t DATA_ALIGNMENT = KV3Helpers::PackAlignOf<Element_t>();

	CKeyValues3Array( int cluster_elem = -1, int alloc_size = DATA_SIZE );
	~CKeyValues3Array();

	int GetClusterElement() const;
	void SetClusterElement( int element );

	CKeyValues3ArrayCluster* GetCluster() const;
	CKV3Arena* GetContext() const;

	Element_t *Base();
	Element_t const *Base() const;

	Element_t Element( int i );
	const Element_t Element( int i ) const { return const_cast<CKeyValues3Array*>(this)->Element( i ); }
	int Count() const;

	void EnsureElementCapacity( int count, bool force = false, bool dont_move = false );

	void SetCount( KeyValues3 *parent, int count, KV3TypeEx_t type = KV3_TYPEEX_NULL, KV3SubType_t subtype = KV3_SUBTYPE_UNSPECIFIED );
	Element_t* InsertMultipleBefore( KeyValues3 *parent, int from, int num );
	void CopyFrom( KeyValues3 *parent, const CKeyValues3Array* pSrc );
	void RemoveMultiple( KeyValues3 *parent, int from, int num );

	void Free( bool clearing_context = false );
	void PurgeContent( KeyValues3 *parent, bool clearing_context = false );
	void PurgeBuffers();

	static constexpr size_t TotalSizeOf( int initial_size ) { return ALIGN_VALUE( TotalSizeWithoutStaticData() + TotalSizeOfData( MAX( initial_size, 0 ) ), 8 ); }
	static constexpr size_t TotalSizeOfData( int size ) { return MAX( (KV3Helpers::PackSizeOf<DATA_ALIGNMENT, Element_t>( size )), sizeof( m_pDynamicElements ) ); }
	static constexpr size_t TotalSizeWithoutStaticData() { return sizeof( CKeyValues3Array ) - sizeof( m_StaticElements ); }

	size_t GetAllocatedSize() const;

private:
	int GetAllocatedChunks() const;
	bool IsBaseStatic();

	size_t GetAllocatedBytesSize() const;

private:
	int m_nClusterElement;
	int m_nAllocatedChunks;

	int m_nCount;
	uint8 m_nInitialSize;
	bool m_bIsDynamicallySized;

	union
	{
		Element_t m_StaticElements[DATA_SIZE];
		Element_t *m_pDynamicElements;
	};
};
COMPILE_TIME_ASSERT(sizeof(CKeyValues3Array) == 64);

class CKeyValues3Table
{
public:
	enum
	{
		MEMBER_FLAG_EXTERNAL_NAME = (1 << 0),
		MEMBER_FLAG_LARGE_SYMBOL = (1 << 1)
	};

	typedef CUtlStringToken	Hash_t;
	typedef KeyValues3*		Member_t;
	union Name_t
	{
		const char *m_String;
		UtlSymLargeId_t m_SymId;
	};
	typedef uint8			Flags_t;

	static const size_t DATA_SIZE = KV3_TABLE_MAX_FIXED_MEMBERS;
	static const size_t DATA_ALIGNMENT = KV3Helpers::PackAlignOf<Hash_t, Member_t, Name_t, Flags_t>();

	CKeyValues3Table( int cluster_elem = -1, int alloc_size = DATA_SIZE );
	~CKeyValues3Table();

	int GetClusterElement() const;
	void SetClusterElement( int element );

	CKeyValues3TableCluster* GetCluster() const;
	CKV3Arena* GetContext() const;

	int GetMemberCount() const;
	Member_t GetMember( KV3MemberId_t id );
	const Member_t GetMember( KV3MemberId_t id ) const { return const_cast<CKeyValues3Table*>(this)->GetMember( id ); }
	const char *GetMemberName( const KeyValues3 *parent, KV3MemberId_t id ) const;
	const Hash_t GetMemberHash( KV3MemberId_t id ) const;
	CKV3MemberName GetKV3MemberName( const KeyValues3 *parent, KV3MemberId_t id ) const;

	bool HasInvalidMemberNames() const;
	void SetHasInvalidMemberNames( bool state );

	void PurgeFastSearch();
	void EnableFastSearch();
	void EnsureMemberCapacity( int num, bool force = false, bool dont_move = false );

	KV3MemberId_t FindMember( const KeyValues3* kv ) const;
	KV3MemberId_t FindMember( const CKV3MemberName &name );
	KV3MemberId_t FindMember( const CKV3MemberName &name, KV3MemberId_t &hint );
	KV3MemberId_t CreateMember( KeyValues3 *parent, const CKV3MemberName &name, bool name_external = false );

	void CopyFrom( KeyValues3 *parent, const KeyValues3 *src_parent, const CKeyValues3Table* src );
	void RemoveMember( KeyValues3 *parent, KV3MemberId_t id );
	void RenameMember( KeyValues3 *parent, KV3MemberId_t id, const CKV3MemberName &newName );
	void RemoveAll( KeyValues3 *parent, int new_size = 0 );

	void Free( bool clearing_context = false );
	void PurgeContent( KeyValues3 *parent, bool bClearingContext = false );
	void PurgeBuffers();

	static constexpr size_t TotalSizeOf( int initial_size ) { return ALIGN_VALUE( TotalSizeWithoutStaticData() + TotalSizeOfData( MAX( initial_size, 0 ) ), 8 ); }
	static constexpr size_t TotalSizeOfData( int size ) { return MAX( (KV3Helpers::PackSizeOf<DATA_ALIGNMENT, Hash_t, Member_t, Name_t, Flags_t>( size )), sizeof(m_pDynamicBuffer) ); }
	static constexpr size_t TotalSizeWithoutStaticData() { return sizeof(CKeyValues3Table) - sizeof(m_StaticBuffer); }

	size_t GetAllocatedSize() const;

private:
	void PurgeNameBuffer( KeyValues3 *parent, KV3MemberId_t id );

	void StoreKeyName( KeyValues3 *parent, Name_t &out_string, Flags_t &out_flags, const char *input_string, UtlSymLargeId_t sym_id = UTL_INVAL_SYMBOL_LARGE, bool name_external = false );

	int GetAllocatedChunks() const;
	bool IsBaseStatic();

	size_t GetAllocatedBytesSize() const;

	constexpr size_t OffsetToHashesBase( int size ) const { return 0; }
	constexpr size_t OffsetToMembersBase( int size ) const { return KV3Helpers::PackSizeOf<DATA_ALIGNMENT, Hash_t>( size ); }
	constexpr size_t OffsetToNamesBase( int size ) const { return KV3Helpers::PackSizeOf<DATA_ALIGNMENT, Hash_t, Member_t>( size ); }
	constexpr size_t OffsetToFlagsBase( int size ) const { return KV3Helpers::PackSizeOf<DATA_ALIGNMENT, Hash_t, Member_t, Name_t>( size ); }

	// Gets the base address (can change when adding elements!)
	void *Base();
	Hash_t *HashesBase();
	Member_t *MembersBase();
	Name_t *NamesBase();
	Flags_t *FlagsBase();

	const void *Base() const;
	const Hash_t *HashesBase() const;
	const Member_t *MembersBase() const;
	const Name_t *NamesBase() const;
	const Flags_t *FlagsBase() const;

private:
	int m_nClusterElement;
	int m_nAllocatedChunks;

	struct kv3tablefastsearch_t {
		kv3tablefastsearch_t();
		~kv3tablefastsearch_t();

		void Clear();

		struct EmptyHashFunctor { unsigned int operator()( uint32 n ) const; };
		typedef CUtlHashtable<unsigned int, KV3MemberId_t, EmptyHashFunctor> Hashtable_t;

		bool		m_ignore;
		int8		m_ignores_counter;
		Hashtable_t	m_member_ids;
	} *m_pFastSearch;

	int m_nCount;

	uint8 m_nInitialSize;
	bool m_bIsDynamicallySized;

	bool m_bHasInvalidMemberNames;
	// AMNOTE: Zeroed when the table is set up, nothing reads it
	bool m_unk001;

	union
	{
		struct
		{
			Hash_t m_Hashes[DATA_SIZE];
			Member_t m_Members[DATA_SIZE];
			Name_t m_Names[DATA_SIZE];
			Flags_t m_Flags[DATA_SIZE];
		} m_StaticBuffer;

		void* m_pDynamicBuffer;
	};
};
COMPILE_TIME_ASSERT(sizeof(CKeyValues3Table) == 192);

template <size_t SIZE, typename T>
class CKeyValues3ClusterImpl
{
public:
	typedef T NodeType;
	static const size_t CLUSTER_SIZE = SIZE;

	union Node
	{
		Node() : m_pNextFree( nullptr ) {}
		~Node() {}

		NodeType m_Value;
		Node *m_pNextFree;
	};

	enum
	{
		HEAP_MARKER = (1 << 31),

		FLAGS_MASK = ~(HEAP_MARKER)
	};

	CKeyValues3ClusterImpl( CKV3Arena *context, bool allocated_on_heap = false, int initial_size = SIZE );
	~CKeyValues3ClusterImpl() { Purge(); }

	CKV3Arena *GetContext() const { return m_pContext; }

	bool IsFull() const { return NumCount() >= NumAllocated(); }
	bool IsAllocatedOnHeap() const { return (m_nAllocatedElements & HEAP_MARKER) != 0; }
	int NumAllocated() const { return m_nAllocatedElements & FLAGS_MASK; }
	int NumCount() const { return m_nElementCount; }

	template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<NodeType, Args...>, int>>
	NodeType *Alloc( Args&&... args );

	void Free( int element, bool clearing_context = false );
	void Free( NodeType *node, bool clearing_context = false );

	void Purge();
	void Clear();

	Node *GetNextFree() const { return m_pFirstFreeNode; }
	void SetNextFree( Node *free ) { m_pFirstFreeNode = free; }

	CKeyValues3ClusterImpl *GetNext() const { return m_pNext; }
	void SetNext( CKeyValues3ClusterImpl *cluster ) { m_pNext = cluster; }

	CKeyValues3ClusterImpl *GetPrev() const { return m_pPrev; }
	void SetPrev( CKeyValues3ClusterImpl *cluster ) { m_pPrev = cluster; }

	Node *Head() { return &m_Values[0]; }
	Node *Tail() { return &m_Values[NumAllocated()]; }

	const Node *Head() const { return const_cast<CKeyValues3ClusterImpl *>(this)->Head(); }
	const Node *Tail() const { return const_cast<CKeyValues3ClusterImpl *>(this)->Tail(); }

	void EnableMetaData( bool bEnable );
	void ClearMetaData();
	void PurgeMetaData();
	void PurgeMetaData( int element );
	KV3MetaData_t *GetMetaData( int element ) const;

	int GetNodeIndex( NodeType *node ) const;

	static constexpr size_t TotalSizeOf( int initial_size ) { return ALIGN_VALUE( TotalSizeWithoutStaticData() + TotalSizeOfData( MAX( initial_size, 0 ) ), 8 ); }
	static constexpr size_t TotalSizeOfData( int size ) { return sizeof( Node ) * size; }
	static constexpr size_t TotalSizeWithoutStaticData() { return sizeof( CKeyValues3ClusterImpl ) - TotalSizeOfData( SIZE ); }

	friend CKeyValues3Cluster *KeyValues3::GetCluster() const;
	friend CKeyValues3ArrayCluster *CKeyValues3Array::GetCluster() const;
	friend CKeyValues3TableCluster *CKeyValues3Table::GetCluster() const;

private:
	void InitNodes();
	void PurgeNodes( bool clearing_context = false );

private:
	struct kv3metadata_t
	{
		int m_AllocatedElements;
		KV3MetaData_t m_elements[SIZE];
	};

	CKV3Arena *m_pContext;
	Node *m_pFirstFreeNode;

	int m_nAllocatedElements;
	int m_nElementCount;

	CKeyValues3ClusterImpl *m_pPrev;
	CKeyValues3ClusterImpl *m_pNext;

	kv3metadata_t *m_pMetaData;

	Node m_Values[SIZE];
};

class CKV3ArenaBase
{
public:
	CKV3ArenaBase( CKV3Arena* context );
	~CKV3ArenaBase();

	void Clear();
	void Purge();

protected:
	template <typename CLUSTER>
	struct ClusterNodeChain
	{
		ClusterNodeChain() : m_pTail( nullptr ), m_pHead( nullptr )
		{}

		void Reset()
		{
			m_pTail = nullptr;
			m_pHead = nullptr;
		}

		void AddToChain( CLUSTER *cluster );
		void RemoveFromChain( CLUSTER *cluster );

		CLUSTER *m_pTail;
		CLUSTER *m_pHead;
	};

	template <typename NODE>
	class NodeList
	{
	public:
		struct ListEntry
		{
			ListEntry *m_pNext;
			NODE m_Value;
		};

		NodeList() : m_nUsedBytes( 0 ), m_nAllocatedBytes( 0 ), m_pData( nullptr )
		{}

		~NodeList() { Free(); }

		NODE *Alloc( int initial_size );
		void Free() { Purge(); }
		void Purge();
		void Clear();

		int UsedBytes() const { return m_nUsedBytes; }
		int AllocatedBytes() const { return m_nAllocatedBytes; }
		int FreeBytes() const { return m_nAllocatedBytes - m_nUsedBytes; }

		bool IsFull() const { return m_nUsedBytes >= m_nAllocatedBytes; }

		ListEntry *Head() { return &m_pData[0]; }
		ListEntry *Tail() { return reinterpret_cast<ListEntry *>((uint8 *)Head() + m_nUsedBytes); }

		bool IsWithinRange( NODE *element ) { return AllocatedBytes() > 0 && element >= (void *)Head() && element < (void *)Tail(); }

	private:
		void EnsureByteSize( int bytes_needed );

	private:
		int m_nUsedBytes;
		int m_nAllocatedBytes;
		ListEntry *m_pData;
	};

	CKV3Arena* m_pContext;
	CUtlBuffer m_BinaryData;

	CKeyValues3Cluster m_KV3BaseCluster;

	ClusterNodeChain<CKeyValues3Cluster> m_KV3PartialClusters;
	ClusterNodeChain<CKeyValues3Cluster> m_KV3FullClusters;

	ClusterNodeChain<CKeyValues3ArrayCluster> m_PartialArrayClusters;
	ClusterNodeChain<CKeyValues3ArrayCluster> m_FullArrayClusters;
	NodeList<CKeyValues3Array> m_RawArrayEntries;

	ClusterNodeChain<CKeyValues3TableCluster> m_PartialTableClusters;
	ClusterNodeChain<CKeyValues3TableCluster> m_FullTableClusters;
	NodeList<CKeyValues3Table> m_RawTableEntries;

	CUtlSymbolTableLarge m_Symbols;

	bool m_bMetaDataEnabled: 1;
	bool m_bFormatConverted: 1;
	bool m_bRootAvailabe: 1;

	IErrorListener* m_pParsingErrorListener;

	friend class KeyValues3;
};

class CKV3Arena : public CKV3ArenaBase
{
	typedef CKV3ArenaBase BaseClass;

public:
	CKV3Arena( bool bNoRoot = false );
	~CKV3Arena();

	KeyValues3* AllocKV( KV3TypeEx_t type = KV3_TYPEEX_NULL, KV3SubType_t subtype = KV3_SUBTYPE_UNSPECIFIED );
	// WARNING: kv must belong to this context!!!
	void FreeKV( KeyValues3* kv );
	
	// gets the pre-allocated kv if we indicated its existence when creating the context
	KeyValues3* Root();
	const KeyValues3* Root() const { return const_cast<CKV3Arena*>(this)->Root(); }

	bool IsMetaDataEnabled() const;
	// returns true if the desired format was converted to another after loading via LoadKV3*
	bool IsFormatConverted() const;
	bool IsRootAvailabe() const;

	// filled in after loading via LoadKV3* in binary encoding
	CUtlBuffer& GetBinaryData();

	IErrorListener* GetParsingErrorListener() const;
	void SetParsingErrorListener( IErrorListener* listener );

	const char* AllocString( const char* pString, UtlSymLargeId_t *symid = NULL );
	const char *LookupString( UtlSymLargeId_t symid ) const;

	void EnableMetaData( bool bEnable );
	void CopyMetaData( KV3MetaData_t* pDest, const KV3MetaData_t* pSrc );

	void Clear();
	void Purge();

	template <typename CLUSTER>
	void ClearClusterNodeChain( ClusterNodeChain<CLUSTER> &cluster_node );
	template <typename CLUSTER>
	void PurgeClusterNodeChain( ClusterNodeChain<CLUSTER> &cluster_node );

	bool IsArrayRawAllocated( CKeyValues3Array *element );
	bool IsTableRawAllocated( CKeyValues3Table *element );

private:
	template <typename CLUSTER>
	void MoveToPartial( ClusterNodeChain<CLUSTER> &full_cluster, ClusterNodeChain<CLUSTER> &partial_cluster );

	template <typename CLUSTER, typename... Args, typename = typename std::enable_if_t<std::is_constructible_v<typename CLUSTER::NodeType, Args...>, int>>
	auto Alloc( ClusterNodeChain<CLUSTER> &partial_clusters, ClusterNodeChain<CLUSTER> &full_clusters, int initial_size, Args&&... args );

	template <typename CLUSTER, typename NODE, typename... Args, typename = typename std::enable_if_t<std::is_constructible_v<typename CLUSTER::NodeType, Args...>, int>>
	NODE *RawAlloc( NodeList<NODE> &raw_array, ClusterNodeChain<CLUSTER> &partial_clusters, ClusterNodeChain<CLUSTER> &full_clusters, int initial_size, Args&&... args );

	CKeyValues3Array *AllocArray( int initial_size = 0 );
	CKeyValues3Table *AllocTable( int initial_size = 0 );

	template<typename CLUSTER, typename NODE>
	void Free( NODE *element, ClusterNodeChain<CLUSTER> &partial_clusters, ClusterNodeChain<CLUSTER> &full_clusters );

	void FreeArray( CKeyValues3Array *element );
	void FreeTable( CKeyValues3Table *element );

private:
	uint8 pad[ KV3_CONTEXT_SIZE - ( sizeof( BaseClass ) % KV3_CONTEXT_SIZE ) ];

	friend class KeyValues3;
};
COMPILE_TIME_ASSERT(sizeof(CKV3Arena) == KV3_CONTEXT_SIZE);

template < typename T > inline T KeyValues3::FromString( T defaultValue ) const { Assert( 0 ); return defaultValue; }
template <> inline bool KeyValues3::FromString( bool defaultValue ) const		{ return V_StringToBool( GetString(), defaultValue ); }
template <> inline int8_t KeyValues3::FromString( int8_t defaultValue ) const		{ return V_StringToInt8( GetString(), defaultValue, NULL, NULL, PARSING_FLAG_USE_PARTIAL_RESULT ); }
template <> inline uint8_t KeyValues3::FromString( uint8_t defaultValue ) const		{ return V_StringToUint8( GetString(), defaultValue, NULL, NULL, PARSING_FLAG_USE_PARTIAL_RESULT ); }
template <> inline int16_t KeyValues3::FromString( int16_t defaultValue ) const		{ return V_StringToInt16( GetString(), defaultValue, NULL, NULL, PARSING_FLAG_USE_PARTIAL_RESULT ); }
template <> inline uint16_t KeyValues3::FromString( uint16_t defaultValue ) const	{ return V_StringToUint16( GetString(), defaultValue, NULL, NULL, PARSING_FLAG_USE_PARTIAL_RESULT ); }
template <> inline int32_t KeyValues3::FromString( int32_t defaultValue ) const		{ return V_StringToInt32( GetString(), defaultValue, NULL, NULL, PARSING_FLAG_USE_PARTIAL_RESULT ); }
template <> inline uint32_t KeyValues3::FromString( uint32_t defaultValue ) const	{ return V_StringToUint32( GetString(), defaultValue, NULL, NULL, PARSING_FLAG_USE_PARTIAL_RESULT ); }
template <> inline int64_t KeyValues3::FromString( int64_t defaultValue ) const		{ return V_StringToInt64( GetString(), defaultValue, NULL, NULL, PARSING_FLAG_USE_PARTIAL_RESULT ); }
template <> inline uint64_t KeyValues3::FromString( uint64_t defaultValue ) const	{ return V_StringToUint64( GetString(), defaultValue, NULL, NULL, PARSING_FLAG_USE_PARTIAL_RESULT ); }
template <> inline float KeyValues3::FromString( float defaultValue ) const	{ return V_StringToFloat32( GetString(), defaultValue, NULL, NULL, PARSING_FLAG_USE_PARTIAL_RESULT ); }
template <> inline double KeyValues3::FromString( double defaultValue ) const	{ return V_StringToFloat64( GetString(), defaultValue, NULL, NULL, PARSING_FLAG_USE_PARTIAL_RESULT ); }

template < typename T > inline void KeyValues3::SetDirect( T value ) { Assert( 0 ); }
template <> inline void KeyValues3::SetDirect( bool value )		{ m_Data.m_Bool = value; }
template <> inline void KeyValues3::SetDirect( int8_t value )		{ m_Data.m_Int = ( int64_t )value; }
template <> inline void KeyValues3::SetDirect( uint8_t value )	{ m_Data.m_UInt = ( uint64_t )value; }
template <> inline void KeyValues3::SetDirect( int16_t value )	{ m_Data.m_Int = ( int64_t )value; }
template <> inline void KeyValues3::SetDirect( uint16_t value )	{ m_Data.m_UInt = ( uint64_t )value; }
template <> inline void KeyValues3::SetDirect( int32_t value )	{ m_Data.m_Int = ( int64_t )value; }
template <> inline void KeyValues3::SetDirect( uint32_t value )	{ m_Data.m_UInt = ( uint64_t )value; }
template <> inline void KeyValues3::SetDirect( int64_t value )	{ m_Data.m_Int = value; }
template <> inline void KeyValues3::SetDirect( uint64_t value )	{ m_Data.m_UInt = value; }
template <> inline void KeyValues3::SetDirect( float value )	{ m_Data.m_Double = ( double )value; }
template <> inline void KeyValues3::SetDirect( double value )	{ m_Data.m_Double = value; }

template < typename T >
T KeyValues3::GetVecBasedObj( int size, const T &defaultValue ) const
{
	T obj;
	if ( !ReadArrayFloat32( size, obj.Base() ) )
		obj = defaultValue;
	return obj;
}

template < typename T >
void KeyValues3::SetVecBasedObj( const T &obj, int size, KV3SubType_t subtype )
{
	AllocArray<float32>( size, obj.Base(), KV3_ARRAY_ALLOC_NORMAL, KV3_TYPEEX_INVALID, KV3_TYPEEX_ARRAY_FLOAT32, subtype, KV3_TYPEEX_DOUBLE, KV3_SUBTYPE_FLOAT32 );
}

template < typename T >
T KeyValues3::GetValue( T defaultValue ) const
{
	switch ( GetType() )
	{
		case KV3_TYPE_BOOL:
			return ( T )m_Data.m_Bool;
		case KV3_TYPE_INT:
			return ( T )m_Data.m_Int;
		case KV3_TYPE_UINT:
			return ( GetSubType() != KV3_SUBTYPE_POINTER ) ? ( T )m_Data.m_UInt : defaultValue;
		case KV3_TYPE_DOUBLE:
			if constexpr ( std::is_same_v<T, bool> )
				return ( int64 )m_Data.m_Double != 0;
			else
				return ( T )m_Data.m_Double;
		case KV3_TYPE_STRING:
			return FromString<T>( defaultValue );
		default:
			return defaultValue;
	}
}

template < typename T >
void KeyValues3::SetValue( T value, KV3TypeEx_t type, KV3SubType_t subtype )
{
	PrepareForType( type, subtype );
	SetDirect<T>( value );
}

template < typename T >
void KeyValues3::NormalizeArray( KV3TypeEx_t type, KV3SubType_t subtype, int size, const T* data, bool bFree )
{
	m_TypeEx = KV3_TYPEEX_ARRAY;
	Alloc( size );

	m_Data.m_pArray->SetCount( this, size, type, subtype );

	CKeyValues3Array::Element_t* arr = m_Data.m_pArray->Base();
	for ( int i = 0; i < m_Data.m_pArray->Count(); ++i )
		arr[ i ]->SetDirect( data[ i ] );

	if ( bFree )
		free( (void*)data );
}

template<typename T>
inline T *KeyValues3::AllocateOnHeap( int initial_size )
{
	if(initial_size <= 0)
		initial_size = T::DATA_SIZE;

	auto element = (T *)g_pMemAlloc->RegionAlloc( MemAllocRegion::AllocKV3, T::TotalSizeOf( initial_size ) );
	Construct( element, -1, initial_size );

	return element;
}

template<typename T>
inline void KeyValues3::FreeOnHeap( T *element )
{
	Destruct( element, element->GetAllocatedSize() );

	g_pMemAlloc->RegionFree( MemAllocRegion::FreeKV3, element );
}

template < typename T >
void KeyValues3::AllocArray( int size, const T* data, KV3ArrayAllocType_t alloc_type, KV3TypeEx_t type_short, KV3TypeEx_t type_ptr, KV3SubType_t subtype, KV3TypeEx_t type_elem, KV3SubType_t subtype_elem )
{
	int nMaxSizeShort = sizeof( uint64 ) / sizeof( T );

	if ( type_short != KV3_TYPEEX_INVALID && size <= nMaxSizeShort )
	{
		if ( alloc_type == KV3_ARRAY_ALLOC_EXTERN && type_ptr != KV3_TYPEEX_INVALID )
		{
			PrepareForType( type_ptr, subtype );

			m_bFreeArrayMemory = false;
			m_nNumArrayElements = size;
			m_Data.m_pMemory = (void*)data;
		}
		else
		{
			PrepareForType( type_short, subtype );

			m_bFreeArrayMemory = false;
			m_nNumArrayElements = size;
			m_Data.m_pMemory = NULL;
			memcpy( &m_Data.m_pMemory, data, size * sizeof( T ) );

			if ( alloc_type == KV3_ARRAY_ALLOC_EXTERN_FREE )
				free( (void*)data );
		}
	}
	else if ( type_ptr != KV3_TYPEEX_INVALID && size <= 31 )
	{
		PrepareForType( type_ptr, subtype );

		m_nNumArrayElements = size;

		if ( alloc_type == KV3_ARRAY_ALLOC_EXTERN )
		{
			m_bFreeArrayMemory = false;
			m_Data.m_pMemory = (void*)data;
		}
		else if ( alloc_type == KV3_ARRAY_ALLOC_EXTERN_FREE )
		{
			m_bFreeArrayMemory = true;
			m_Data.m_pMemory = (void*)data;
		}
		else
		{
			m_bFreeArrayMemory = true;	
			m_Data.m_pMemory = malloc( size * sizeof( T ) );
			memcpy( m_Data.m_pMemory, data, size * sizeof( T ) );
		}
	}
	else
	{
		PrepareForType( KV3_TYPEEX_ARRAY, subtype, size );

		m_Data.m_pArray->SetCount( this, size, type_elem, subtype_elem );

		CKeyValues3Array::Element_t* arr = m_Data.m_pArray->Base();
		for ( int i = 0; i < m_Data.m_pArray->Count(); ++i )
			arr[ i ]->SetValue<T>( data[ i ], type_elem, subtype_elem );

		if ( alloc_type == KV3_ARRAY_ALLOC_EXTERN_FREE )
			free( (void*)data );
	}
}

template<size_t SIZE, typename T>
inline CKeyValues3ClusterImpl<SIZE, T>::CKeyValues3ClusterImpl( CKV3Arena *context, bool allocated_on_heap, int initial_size ) :
	m_pContext( context ),
	m_pFirstFreeNode( nullptr ),
	m_nAllocatedElements( initial_size | (allocated_on_heap ? HEAP_MARKER : 0) ),
	m_nElementCount( 0 ),
	m_pPrev( nullptr ),
	m_pNext( nullptr ),
	m_pMetaData( nullptr )
{
	InitNodes();
}

template<size_t SIZE, typename T>
template<typename... Args, typename>
inline T *CKeyValues3ClusterImpl<SIZE, T>::Alloc( Args&&... args )
{
	Assert( !IsFull() );

	Node *node = GetNextFree();
	Assert( node != nullptr );

	SetNextFree( node->m_pNextFree );

	Construct( &node->m_Value, std::forward<Args>( args )... );
	node->m_Value.SetClusterElement( GetNodeIndex( &node->m_Value ) );

	m_nElementCount++;

	return &node->m_Value;
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::Free( NodeType* node, bool clearing_context )
{
	Assert( node >= (void *)Head() && node < (void *)Tail() );
	Free( GetNodeIndex( node ), clearing_context );
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::Free( int element, bool clearing_context )
{
	Assert( element >= 0 && element < NumAllocated() );

	Node *node = &m_Values[element];
	node->m_Value.Free( clearing_context );

	m_nElementCount--;

	node->m_pNextFree = GetNextFree();
	SetNextFree( node );
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::InitNodes()
{
	Node *iter = Tail() - 1;
	Node *prev = nullptr;

	for(int i = 0; i < NumAllocated(); i++, iter--)
	{
		iter->m_pNextFree = prev;
		prev = iter;
	}

	m_nElementCount = 0;
	SetNextFree( prev );
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::PurgeNodes( bool clearing_context )
{
	CVarBitVec free_nodes( NumAllocated() );

	for(auto iter = GetNextFree(); iter; iter = iter->m_pNextFree)
	{
		free_nodes.Set( GetNodeIndex( &iter->m_Value ) );
	}

	if(!free_nodes.IsAllSet())
	{
		for(int i = 0; i < NumAllocated(); i++)
		{
			if(!free_nodes.IsBitSet( i ))
			{
				Free( i, clearing_context );
			}
		}

		InitNodes();
	}
}

template<size_t SIZE, typename T>
inline int CKeyValues3ClusterImpl<SIZE, T>::GetNodeIndex( NodeType *element ) const
{
	Node *node = reinterpret_cast<Node *>(element);

	auto head = Head();
	if(node < head || node >= Tail())
		return -1;

	return node - head;
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::Purge()
{
	PurgeNodes( true );
	PurgeMetaData();
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::Clear()
{
	PurgeNodes( true );
	ClearMetaData();
}

template<size_t SIZE, typename T>
void CKeyValues3ClusterImpl<SIZE, T>::EnableMetaData( bool bEnable )
{
	if(bEnable)
	{
		if(!m_pMetaData)
		{
			m_pMetaData = (kv3metadata_t *)g_pMemAlloc->RegionAlloc( MemAllocRegion::AllocKV3, (NumAllocated() * sizeof(KV3MetaData_t)) + 8 );
			m_pMetaData->m_AllocatedElements = NumAllocated();

			for(int i = 0; i < m_pMetaData->m_AllocatedElements; i++)
			{
				Construct( &m_pMetaData->m_elements[i] );
			}
		}
	}
	else
	{
		PurgeMetaData();
	}
}

template<size_t SIZE, typename T>
void CKeyValues3ClusterImpl<SIZE, T>::ClearMetaData()
{
	if(m_pMetaData)
	{
		for(int i = 0; i < m_pMetaData->m_AllocatedElements; i++)
		{
			m_pMetaData->m_elements[i].Clear();
		}
	}
}

template<size_t SIZE, typename T>
void CKeyValues3ClusterImpl<SIZE, T>::PurgeMetaData()
{
	if(m_pMetaData)
	{
		for(int i = 0; i < m_pMetaData->m_AllocatedElements; i++)
		{
			Destruct( &m_pMetaData->m_elements[i] );
		}

		g_pMemAlloc->RegionFree( MemAllocRegion::FreeKV3, m_pMetaData );
	}

	m_pMetaData = nullptr;
}

template<size_t SIZE, typename T>
void CKeyValues3ClusterImpl<SIZE, T>::PurgeMetaData( int element )
{
	if(!m_pMetaData)
		return;

	Assert( element >= 0 && element < m_pMetaData->m_AllocatedElements );
	GetMetaData( element )->Clear();
}

template<size_t SIZE, typename T>
KV3MetaData_t *CKeyValues3ClusterImpl<SIZE, T>::GetMetaData( int element ) const
{
	if(!m_pMetaData)
		return nullptr;

	Assert( element >= 0 && element < m_pMetaData->m_AllocatedElements );
	return &m_pMetaData->m_elements[element];
}

template<typename CLUSTER>
inline void CKV3ArenaBase::ClusterNodeChain<CLUSTER>::AddToChain( CLUSTER *cluster )
{
	if(m_pTail)
		m_pTail->SetNext( cluster );
	else
		m_pHead = cluster;

	cluster->SetNext( nullptr );
	cluster->SetPrev( m_pTail );

	m_pTail = cluster;
}

template<typename CLUSTER>
inline void CKV3ArenaBase::ClusterNodeChain<CLUSTER>::RemoveFromChain( CLUSTER *cluster )
{
	auto prev = cluster->GetPrev();
	auto next = cluster->GetNext();

	if(prev)
		prev->SetNext( next );
	else
		m_pHead = next;

	if(next)
		next->SetPrev( prev );
	else
		m_pTail = prev;

	cluster->SetPrev( nullptr );
	cluster->SetNext( nullptr );
}

template<typename NODE>
inline void CKV3ArenaBase::NodeList<NODE>::EnsureByteSize( int bytes_needed )
{
	if(bytes_needed <= m_nAllocatedBytes)
		return;

	int new_alloc_size = CalcNewDoublingCount<int>( m_nAllocatedBytes, bytes_needed, ALLOC_CONTEXT_NODELIST_MIN, ALLOC_CONTEXT_NODELIST_MAX );

	m_pData = (ListEntry *)realloc( m_pData, new_alloc_size );
	m_nAllocatedBytes = new_alloc_size;
}

template<typename NODE>
inline NODE *CKV3ArenaBase::NodeList<NODE>::Alloc( int initial_size )
{
	int byte_size_needed = m_nUsedBytes + (int)NODE::TotalSizeOf( initial_size ) + 8;
	EnsureByteSize( byte_size_needed );
	
	auto entry = Tail();
	m_nUsedBytes = byte_size_needed;

	Construct( &entry->m_Value, -1, initial_size );
	entry->m_pNext = Tail();

	return &entry->m_Value;
}

template<typename NODE>
inline void CKV3ArenaBase::NodeList<NODE>::Clear()
{
	if(m_nAllocatedBytes > 0)
	{
		for(auto iter = Head(); iter != Tail(); )
		{
			auto next = iter->m_pNext;
			Destruct( &iter->m_Value, iter->m_Value.GetAllocatedSize() );
			iter = next;
		}
	}

	m_nUsedBytes = 0;
}

template<typename NODE>
inline void CKV3ArenaBase::NodeList<NODE>::Purge()
{
	Clear();

	free( m_pData );
	m_pData = nullptr;
}

template<typename CLUSTER>
inline void CKV3Arena::PurgeClusterNodeChain( ClusterNodeChain<CLUSTER> &cluster_node )
{
	CLUSTER *prev = nullptr;
	for(auto node = cluster_node.m_pTail; node; node = prev)
	{
		prev = node->GetPrev();

		if(node->IsAllocatedOnHeap())
		{
			node->Purge();
			g_pMemAlloc->RegionFree( MemAllocRegion::FreeKV3, node );
		}
		else
		{
			node->Clear();
		}
	}

	cluster_node.Reset();
}

template<typename CLUSTER>
inline void CKV3Arena::ClearClusterNodeChain( ClusterNodeChain<CLUSTER> &cluster_node )
{
	for(auto node = cluster_node.m_pTail; node; node = node->GetPrev())
	{
		node->Clear();
	}
}

template<typename CLUSTER>
inline void CKV3Arena::MoveToPartial( ClusterNodeChain<CLUSTER> &full_cluster, ClusterNodeChain<CLUSTER> &partial_cluster )
{
	CLUSTER *prev;
	for(auto node = full_cluster.m_pTail; node; node = prev)
	{
		prev = node->GetPrev();
		partial_cluster.AddToChain( node );
	}

	full_cluster.Reset();
}

template <typename CLUSTER, typename... Args, typename>
auto CKV3Arena::Alloc( ClusterNodeChain<CLUSTER> &partial_clusters,
								ClusterNodeChain<CLUSTER> &full_clusters,
								int initial_size, Args&&... args )
{
	auto cluster = partial_clusters.m_pTail;
	typename CLUSTER::NodeType *elem = nullptr;

	if(cluster)
	{
		elem = cluster->Alloc( std::forward<Args>( args )... );

		if(cluster->IsFull())
		{
			partial_clusters.RemoveFromChain( cluster );
			full_clusters.AddToChain( cluster );
		}
	}
	else
	{
		cluster = (CLUSTER *)g_pMemAlloc->RegionAlloc( MemAllocRegion::AllocKV3, CLUSTER::TotalSizeOf( initial_size ) );

		Construct( cluster, this, true, initial_size );

		if constexpr(std::is_same_v<CLUSTER, CKeyValues3Cluster>)
			cluster->EnableMetaData( m_bMetaDataEnabled );

		partial_clusters.AddToChain( cluster );

		elem = cluster->Alloc( std::forward<Args>( args )... );
	}

	return elem;
}

template<typename CLUSTER, typename NODE, typename ...Args, typename>
inline NODE *CKV3Arena::RawAlloc( NodeList<NODE> &raw_array, ClusterNodeChain<CLUSTER> &partial_clusters, ClusterNodeChain<CLUSTER> &full_clusters, int initial_size, Args && ...args )
{
	int needed_byte_size = MAX( (int)NODE::TotalSizeOf( initial_size ), 32 );

	if(raw_array.IsFull() || needed_byte_size > raw_array.FreeBytes())
	{
		if(initial_size <= (int)NODE::DATA_SIZE)
			return Alloc( partial_clusters, full_clusters, CLUSTER::CLUSTER_SIZE );
		else
			return nullptr;
	}

	return raw_array.Alloc( initial_size );
}

template<typename CLUSTER, typename NODE>
void CKV3Arena::Free( NODE *element, ClusterNodeChain<CLUSTER> &partial_clusters, ClusterNodeChain<CLUSTER> &full_clusters )
{
	auto cluster = element->GetCluster();

	Assert( cluster != nullptr && cluster->GetContext() == m_pContext );
	
	cluster->Free( element );

	if(cluster->NumCount() > 0)
	{
		if(cluster->NumCount() == cluster->NumAllocated() - 1)
		{
			full_clusters.RemoveFromChain( cluster );
			partial_clusters.AddToChain( cluster );
		}
	}
	else if(cluster->IsAllocatedOnHeap())
	{
		partial_clusters.RemoveFromChain( cluster );

		Destruct( cluster );
		g_pMemAlloc->RegionFree( MemAllocRegion::FreeKV3, cluster );
	}
}

#include "memdbgoff.h"

#endif // KEYVALUES3_H
