#include "public/tier0/keyvalues3.h"
#include "public/tier0/splitstring.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "public/tier0/memdbgon.h"

// Nasty hack to redefine gcc's offsetof which doesn't like GET_OUTER macro
#ifdef COMPILER_GCC
#undef offsetof
#define offsetof(s,m)	(size_t)&(((s *)0)->m)
#endif

// Doubles at least once from no less than min_count until requested_count fits, or gives max_count
static int CalcNewKV3Capacity( int old_count, int requested_count, int min_count, int max_count )
{
	int new_count = MAX( old_count, min_count );

	while(new_count <= max_count / 2)
	{
		new_count *= 2;

		if(new_count >= requested_count)
			return new_count;
	}

	return max_count;
}

// A string made without an arena starts out as an empty short string
KeyValues3::KeyValues3( KV3TypeEx_t type, KV3SubType_t subtype ) :
	KeyValues3( -1, type == KV3_TYPEEX_STRING ? KV3_TYPEEX_STRING_SHORT : type, subtype )
{
}

KeyValues3::KeyValues3( int cluster_elem, KV3TypeEx_t type, KV3SubType_t subtype ) : 
	m_bContextIndependent( true ),
	m_TypeEx( type ),
	m_SubType( subtype ),
	m_nClusterElement( (uint16)-1 ),
	m_nNumArrayElements( 0 ),
	m_nFlags( 0 ),
	m_nReserved( 0 )
{
	SetClusterElement( cluster_elem );
	ResolveUnspecified();
	Alloc();
}

KeyValues3::~KeyValues3() 
{ 
	Free(); 
};

void KeyValues3::Alloc( int initial_size, Data_t data, int preallocated_size, bool should_free )
{
	switch ( GetTypeEx() )
	{
		case KV3_TYPEEX_ARRAY:
		{
			if(preallocated_size <= 0)
			{
				m_Data.m_pArray = AllocArray( initial_size );
				m_bFreeArrayMemory = true;
			}
			else
			{
				AllocArrayInPlace( initial_size, data, preallocated_size, should_free );
			}

			break;
		}
		case KV3_TYPEEX_TABLE:
		{
			if(preallocated_size <= 0)
			{
				m_Data.m_pTable = AllocTable( initial_size );
				m_bFreeArrayMemory = true;
			}
			else
			{
				AllocTableInPlace( initial_size, data, preallocated_size, should_free );
			}
			
			break;
		}
		case KV3_TYPEEX_ARRAY_FLOAT32:
		case KV3_TYPEEX_ARRAY_FLOAT64:
		case KV3_TYPEEX_ARRAY_INT16:
		case KV3_TYPEEX_ARRAY_INT32:
		case KV3_TYPEEX_ARRAY_UINT8_SHORT:
		case KV3_TYPEEX_ARRAY_INT16_SHORT:
		case KV3_TYPEEX_ARRAY_UINT32:
		case KV3_TYPEEX_ARRAY_UINT64:
		{
			m_bFreeArrayMemory = false;
			m_nNumArrayElements = 0;
			m_Data.m_pMemory = nullptr;
			break;
		}
		default: 
			break;
	}
}

void KeyValues3::AllocArrayInPlace( int initial_size, Data_t data, int preallocated_size, bool should_free )
{
	int bytes_needed = (int)MAX( CKeyValues3Array::TotalSizeOf( 0 ), CKeyValues3Array::TotalSizeOf( initial_size ) );

	if(bytes_needed > preallocated_size)
	{
		Plat_FatalError( "KeyValues3: pre-allocated array memory is too small for %u elements (%u bytes available, %u bytes needed)\n", initial_size, preallocated_size, bytes_needed );
		DebuggerBreak();
	}

	Construct( data.m_pArray, -1, initial_size );

	m_Data.m_pArray = data.m_pArray;
	m_bFreeArrayMemory = should_free;
}

void KeyValues3::AllocTableInPlace( int initial_size, Data_t data, int preallocated_size, bool should_free )
{
	int bytes_needed = (int)MAX( CKeyValues3Table::TotalSizeOf( 0 ), CKeyValues3Table::TotalSizeOf( initial_size ) );

	if(bytes_needed > preallocated_size)
	{
		Plat_FatalError( "KeyValues3: pre-allocated table memory is too small for %u members (%u bytes available, %u bytes needed)\n", initial_size, preallocated_size, bytes_needed );
		DebuggerBreak();
	}

	Construct( data.m_pTable, -1, initial_size );

	m_Data.m_pTable = data.m_pTable;
	m_bFreeArrayMemory = should_free;
}

CKeyValues3Array *KeyValues3::AllocArray( int initial_size )
{
	auto context = GetContext();

	if(context)
	{
		auto arr = context->AllocArray( initial_size );

		if(arr)
			return arr;
	}

	return AllocateOnHeap<CKeyValues3Array>( initial_size );
}

CKeyValues3Table* KeyValues3::AllocTable( int initial_size )
{
	auto context = GetContext();

	if(context)
	{
		auto table = context->AllocTable( initial_size );

		if(table)
			return table;
	}

	return AllocateOnHeap<CKeyValues3Table>( initial_size );
}

void KeyValues3::FreeArray( CKeyValues3Array *element, bool clearing_context )
{
	if(!element)
		return;

	element->PurgeContent( this, clearing_context );

	if(!m_bFreeArrayMemory)
	{
		Destruct( element, element->GetAllocatedSize() );
	}
	else
	{
		auto context = GetContext();
		bool raw_allocated = context && context->IsArrayRawAllocated( element );

		if(!raw_allocated && element->GetClusterElement() < 0)
		{
			FreeOnHeap( element );
		}
		else if(!clearing_context)
		{
			if(!raw_allocated)
				context->FreeArray( element );
			else
				Destruct( element, element->GetAllocatedSize() );
		}
	}
}

void KeyValues3::FreeTable( CKeyValues3Table *element, bool clearing_context )
{
	if(!element)
		return;

	element->PurgeContent( this, clearing_context );

	if(!m_bFreeArrayMemory)
	{
		Destruct( element, element->GetAllocatedSize() );
	}
	else
	{
		auto context = GetContext();
		bool raw_allocated = context && context->IsTableRawAllocated( element );

		if(!raw_allocated && element->GetClusterElement() < 0)
		{
			FreeOnHeap( element );
		}
		else if(!clearing_context)
		{
			if(!raw_allocated)
				context->FreeTable( element );
			else
				Destruct( element, element->GetAllocatedSize() );
		}
	}
}

KeyValues3 *KeyValues3::AllocMember( KV3TypeEx_t type, KV3SubType_t subtype )
{
	auto context = GetContext();

	if(context)
		return context->AllocKV( type, subtype );
	else
		return new KeyValues3( type, subtype );
}

void KeyValues3::FreeMember( KeyValues3 *member )
{
	auto context = GetContext();

	if(context)
	{
		auto cluster = member->GetCluster();

		cluster->PurgeMetaData( cluster->GetNodeIndex( member ) );
		context->FreeKV( member );
	}
	else
	{
		delete member;
	}
}

void KeyValues3::Free( bool bClearingContext )
{
	switch ( GetTypeEx() )
	{
		case KV3_TYPEEX_STRING:
		{
			free( (void*)m_Data.m_pString );
			m_Data.m_pString = nullptr;
			break;
		}
		case KV3_TYPEEX_BINARY_BLOB:
		{
			if ( m_Data.m_pBinaryBlob )
				free( m_Data.m_pBinaryBlob );
			m_Data.m_pBinaryBlob = nullptr;
			break;
		}
		case KV3_TYPEEX_BINARY_BLOB_EXTERN:
		{
			if ( m_Data.m_pBinaryBlob )
			{
				if ( m_Data.m_pBinaryBlob->m_bFreeMemory )
					free( (void*)m_Data.m_pBinaryBlob->m_pubData );
				free( m_Data.m_pBinaryBlob );
			}
			m_Data.m_pBinaryBlob = nullptr;
			break;
		}
		case KV3_TYPEEX_ARRAY:
		{
			FreeArray( m_Data.m_pArray, bClearingContext );

			m_bFreeArrayMemory = false;
			m_Data.m_pArray = nullptr;

			break;
		}
		case KV3_TYPEEX_TABLE:
		{
			FreeTable( m_Data.m_pTable, bClearingContext );

			m_bFreeArrayMemory = false;
			m_Data.m_pTable = nullptr;

			break;
		}
		case KV3_TYPEEX_ARRAY_FLOAT32:
		case KV3_TYPEEX_ARRAY_FLOAT64:
		case KV3_TYPEEX_ARRAY_INT16:
		case KV3_TYPEEX_ARRAY_INT32:
		case KV3_TYPEEX_ARRAY_UINT8_SHORT:
		case KV3_TYPEEX_ARRAY_INT16_SHORT:
		case KV3_TYPEEX_ARRAY_UINT32:
		case KV3_TYPEEX_ARRAY_UINT64:
		{
			if ( m_bFreeArrayMemory )
				free( m_Data.m_pMemory );
			m_bFreeArrayMemory = false;
			m_nNumArrayElements = 0;
			m_Data.m_nMemory = 0;
			break;
		}
		default: 
			break;
	}
}

void KeyValues3::ResolveUnspecified()
{
	if ( GetSubType() == KV3_SUBTYPE_UNSPECIFIED )
	{
		switch ( GetType() )
		{
			case KV3_TYPE_NULL:
				m_SubType = KV3_SUBTYPE_NULL;
				break;
			case KV3_TYPE_BOOL:
				m_SubType = KV3_SUBTYPE_BOOL8;
				break;
			case KV3_TYPE_INT:
				m_SubType = KV3_SUBTYPE_INT64;
				break;
			case KV3_TYPE_UINT:
				m_SubType = KV3_SUBTYPE_UINT64;
				break;
			case KV3_TYPE_DOUBLE:
				m_SubType = KV3_SUBTYPE_FLOAT64;
				break;
			case KV3_TYPE_STRING:
				m_SubType = KV3_SUBTYPE_STRING;
				break;
			case KV3_TYPE_BINARY_BLOB:
				m_SubType = KV3_SUBTYPE_BINARY_BLOB;
				break;
			case KV3_TYPE_ARRAY:
				m_SubType = KV3_SUBTYPE_ARRAY;
				break;
			case KV3_TYPE_TABLE:
				m_SubType = KV3_SUBTYPE_TABLE;
				break;
			default:
				m_SubType = KV3_SUBTYPE_INVALID;
				break;
		}
	}
}

void KeyValues3::PrepareForType( KV3TypeEx_t type, KV3SubType_t subtype, int initial_size )
{
	if ( GetTypeEx() == type )
	{
		switch ( type )
		{
			case KV3_TYPEEX_STRING:
			case KV3_TYPEEX_BINARY_BLOB:
			case KV3_TYPEEX_BINARY_BLOB_EXTERN:
			case KV3_TYPEEX_ARRAY_FLOAT32:
			case KV3_TYPEEX_ARRAY_FLOAT64:
			case KV3_TYPEEX_ARRAY_INT16:
			case KV3_TYPEEX_ARRAY_INT32:
			case KV3_TYPEEX_ARRAY_UINT8_SHORT:
			case KV3_TYPEEX_ARRAY_INT16_SHORT:
			case KV3_TYPEEX_ARRAY_UINT32:
			case KV3_TYPEEX_ARRAY_UINT64:
			{
				Free();
				break;
			}
			default: 
				break;
		}
	}
	else
	{
		Free();
		m_TypeEx = type;
		m_Data.m_nMemory = 0;
		Alloc( initial_size );
	}

	m_SubType = subtype;
}

CKeyValues3Cluster* KeyValues3::GetCluster() const
{
	if ( m_bContextIndependent )
		return nullptr;

	return GET_OUTER( CKeyValues3Cluster, m_Values[ m_nClusterElement ] );
}

CKV3Arena* KeyValues3::GetContext() const
{ 
	CKeyValues3Cluster* cluster = GetCluster();

	if ( cluster )
		return cluster->GetContext();
	else
		return nullptr;
}

KV3MetaData_t* KeyValues3::GetMetaData( CKV3Arena** ppCtx ) const
{
	CKeyValues3Cluster* cluster = GetCluster();

	if ( cluster )
	{
		if ( ppCtx )
			*ppCtx = cluster->GetContext();

		return cluster->GetMetaData( m_nClusterElement );
	}
	else
	{
		if ( ppCtx )
			*ppCtx = nullptr;

		return nullptr;
	}
}

const char* KeyValues3::GetString( const char* defaultValue ) const
{
	switch ( GetTypeEx() )
	{
		case KV3_TYPEEX_STRING:
		case KV3_TYPEEX_STRING_EXTERN:
			return m_Data.m_pString;
		case KV3_TYPEEX_STRING_SHORT:
			return m_Data.m_szStringShort;
		case KV3_TYPEEX_STRING_SYMBOL:
		{
			CKV3Arena* context = GetContext();
			const char *str = context ? context->LookupString( m_Data.m_StringSymbol ) : nullptr;
			return str ? str : "";
		}
		default:
			return defaultValue;
	}
}

void KeyValues3::SetString( const char* pString, KV3SubType_t subtype )
{
	if ( !pString )
		pString = "";

	if ( strlen( pString ) < sizeof( m_Data.m_szStringShort ) )
	{
		PrepareForType( KV3_TYPEEX_STRING_SHORT, subtype );
		V_strncpy( m_Data.m_szStringShort, pString, sizeof( m_Data.m_szStringShort ) );
	}
	else
	{
		PrepareForType( KV3_TYPEEX_STRING, subtype );
		m_Data.m_pString = strdup( pString );
	}
}

void KeyValues3::SetStringExternal( const char* pString, KV3SubType_t subtype )
{
	PrepareForType( KV3_TYPEEX_STRING_EXTERN, subtype );
	m_Data.m_pString = pString;
}

const byte* KeyValues3::GetBinaryBlob() const
{
	switch ( GetTypeEx() )
	{
		case KV3_TYPEEX_BINARY_BLOB:
			return m_Data.m_pBinaryBlob ? m_Data.m_pBinaryBlob->m_ubData : nullptr;
		case KV3_TYPEEX_BINARY_BLOB_EXTERN:
			return m_Data.m_pBinaryBlob ? m_Data.m_pBinaryBlob->m_pubData : nullptr;
		default:
			return nullptr;
	}
}

int KeyValues3::GetBinaryBlobSize() const
{
	if ( GetType() != KV3_TYPE_BINARY_BLOB || !m_Data.m_pBinaryBlob )
		return 0;

	return ( int )m_Data.m_pBinaryBlob->m_nSize;
}

void KeyValues3::SetToBinaryBlob( const byte* blob, int size )
{
	PrepareForType( KV3_TYPEEX_BINARY_BLOB, KV3_SUBTYPE_BINARY_BLOB );

	if ( size > 0 )
	{
		m_Data.m_pBinaryBlob = (KV3BinaryBlob_t*)malloc( sizeof( size_t ) + size );
		m_Data.m_pBinaryBlob->m_nSize = size;
		memcpy( m_Data.m_pBinaryBlob->m_ubData, blob, size );
	}
	else
	{
		m_Data.m_pBinaryBlob = nullptr;
	}
}

void KeyValues3::SetToBinaryBlobExternal( const byte* blob, int size, bool free_mem )
{
	PrepareForType( KV3_TYPEEX_BINARY_BLOB_EXTERN, KV3_SUBTYPE_BINARY_BLOB );

	if ( size > 0 )
	{
		m_Data.m_pBinaryBlob = (KV3BinaryBlob_t*)malloc( sizeof( KV3BinaryBlob_t ) );
		m_Data.m_pBinaryBlob->m_nSize = size;
		m_Data.m_pBinaryBlob->m_pubData = blob;
		m_Data.m_pBinaryBlob->m_bFreeMemory = free_mem;
	}
	else
	{
		m_Data.m_pBinaryBlob = nullptr;
	}
}

Color KeyValues3::GetColor( const Color &defaultValue ) const
{
	int32 color[4];
	if ( ReadArrayInt32( 4, color ) )
	{
		return Color( color[0], color[1], color[2], color[3] );
	}
	else if ( ReadArrayInt32( 3, color ) )
	{
		return Color( color[0], color[1], color[2], 255 );
	}
	else
	{
		return defaultValue;
	}
}

void KeyValues3::SetColor( const Color &color )
{
	if ( color.a() == 255 )
		AllocArray<uint8>( 3, &color[0], KV3_ARRAY_ALLOC_NORMAL, KV3_TYPEEX_ARRAY_UINT8_SHORT, KV3_TYPEEX_INVALID, KV3_SUBTYPE_COLOR32, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT8 );
	else
		AllocArray<uint8>( 4, &color[0], KV3_ARRAY_ALLOC_NORMAL, KV3_TYPEEX_ARRAY_UINT8_SHORT, KV3_TYPEEX_INVALID, KV3_SUBTYPE_COLOR32, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT8 );
}

int KeyValues3::GetArrayElementCount() const
{
	if ( GetType() != KV3_TYPE_ARRAY )
		return 0;

	if ( GetTypeEx() == KV3_TYPEEX_ARRAY )
		return m_Data.m_pArray->Count();
	else
		return m_nNumArrayElements;
}

KeyValues3** KeyValues3::GetArrayBase()
{
	NormalizeArray();

	if ( GetTypeEx() != KV3_TYPEEX_ARRAY )
		return nullptr;

	return m_Data.m_pArray->Base();
}

KeyValues3* KeyValues3::GetArrayElement( int elem )
{
	NormalizeArray();

	if ( GetTypeEx() != KV3_TYPEEX_ARRAY )
		return nullptr;

	if ( elem < 0 || elem >= m_Data.m_pArray->Count() )
		return nullptr;

	return m_Data.m_pArray->Element( elem );
}

KeyValues3* KeyValues3::ArrayInsertElementBefore( int elem )
{
	NormalizeArray();

	if ( GetTypeEx() != KV3_TYPEEX_ARRAY )
		return nullptr;

	return *m_Data.m_pArray->InsertMultipleBefore( this, elem, 1 );
}

KeyValues3* KeyValues3::ArrayAddElementToTail()
{
	NormalizeArray();

	if ( GetTypeEx() != KV3_TYPEEX_ARRAY )
		PrepareForType( KV3_TYPEEX_ARRAY, KV3_SUBTYPE_ARRAY );

	return *m_Data.m_pArray->InsertMultipleBefore( this, m_Data.m_pArray->Count(), 1 );
}

void KeyValues3::ArraySwapItems( int idx1, int idx2 )
{
	if(GetTypeEx() != KV3_TYPEEX_ARRAY)
		return;

	if(idx1 < 0 || idx1 >= m_Data.m_pArray->Count())
		return;

	if(idx2 < 0 || idx2 >= m_Data.m_pArray->Count())
		return;

	auto base = GetArrayBase();

	auto temp = base[idx1];
	base[idx1] = base[idx2];
	base[idx2] = temp;
}

void KeyValues3::SetArrayElementCount( int count, KV3TypeEx_t type, KV3SubType_t subtype )
{
	NormalizeArray();

	if ( GetTypeEx() != KV3_TYPEEX_ARRAY )
		PrepareForType( KV3_TYPEEX_ARRAY, KV3_SUBTYPE_ARRAY, count );

	m_Data.m_pArray->SetCount( this, count, type, subtype );
}

void KeyValues3::ArrayRemoveElements( int elem, int num )
{
	if ( GetTypeEx() != KV3_TYPEEX_ARRAY )
		return;

	m_Data.m_pArray->RemoveMultiple( this, elem, num );
}

void KeyValues3::NormalizeArray()
{
	switch ( GetTypeEx() )
	{
		case KV3_TYPEEX_ARRAY_FLOAT32:
		{
			NormalizeArray<float32>( KV3_TYPEEX_DOUBLE, KV3_SUBTYPE_FLOAT32, m_nNumArrayElements, m_Data.m_Array.m_f32, m_bFreeArrayMemory );
			break;
		}
		case KV3_TYPEEX_ARRAY_FLOAT64:
		{
			NormalizeArray<float64>( KV3_TYPEEX_DOUBLE, KV3_SUBTYPE_FLOAT64, m_nNumArrayElements, m_Data.m_Array.m_f64, m_bFreeArrayMemory );
			break;
		}
		case KV3_TYPEEX_ARRAY_INT16:
		{
			NormalizeArray<int16>( KV3_TYPEEX_INT, KV3_SUBTYPE_INT16, m_nNumArrayElements, m_Data.m_Array.m_i16, m_bFreeArrayMemory );
			break;
		}
		case KV3_TYPEEX_ARRAY_INT32:
		{
			NormalizeArray<int32>( KV3_TYPEEX_INT, KV3_SUBTYPE_INT32, m_nNumArrayElements, m_Data.m_Array.m_i32, m_bFreeArrayMemory );
			break;
		}
		case KV3_TYPEEX_ARRAY_UINT8_SHORT:
		{
			uint8 u8ArrayShort[8];
			memcpy( u8ArrayShort, m_Data.m_Array.m_u8Short, sizeof( u8ArrayShort ) );
			NormalizeArray<uint8>( KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT8, m_nNumArrayElements, u8ArrayShort, false );
			break;
		}
		case KV3_TYPEEX_ARRAY_INT16_SHORT:
		{
			int16 i16ArrayShort[4];
			memcpy( i16ArrayShort, m_Data.m_Array.m_u8Short, sizeof( i16ArrayShort ) );
			NormalizeArray<int16>( KV3_TYPEEX_INT, KV3_SUBTYPE_INT16, m_nNumArrayElements, i16ArrayShort, false );
			break;
		}
		case KV3_TYPEEX_ARRAY_UINT32:
		{
			NormalizeArray<uint32>( KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT32, m_nNumArrayElements, m_Data.m_Array.m_u32, m_bFreeArrayMemory );
			break;
		}
		case KV3_TYPEEX_ARRAY_UINT64:
		{
			NormalizeArray<uint64>( KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT64, m_nNumArrayElements, m_Data.m_Array.m_u64, m_bFreeArrayMemory );
			break;
		}
		default: 
			break;
	}
}

bool KeyValues3::ReadArrayInt32( int dest_size, int32* data ) const
{
	int src_size = 0;

	if ( GetType() == KV3_TYPE_STRING )
	{
		CSplitString values( GetString(), " " );
		src_size = values.Count();
		int count = MIN( src_size, dest_size );
		for ( int i = 0; i < count; ++i )
			data[ i ] = V_StringToInt32( values[ i ], 0, NULL, NULL, PARSING_FLAG_SKIP_ASSERT );
	}
	else
	{
		switch ( GetTypeEx() )
		{
			case KV3_TYPEEX_ARRAY:
			{
				src_size = m_Data.m_pArray->Count();
				int count = MIN( src_size, dest_size );
				KeyValues3** arr = m_Data.m_pArray->Base();
				for ( int i = 0; i < count; ++i )
					data[ i ] = arr[ i ]->GetInt();
				break;
			}
			case KV3_TYPEEX_ARRAY_INT16:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( int32 )m_Data.m_Array.m_i16[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_INT32:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				memcpy( data, m_Data.m_Array.m_i32, count * sizeof( int32 ) );
				break;
			}
			case KV3_TYPEEX_ARRAY_UINT8_SHORT:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( int32 )m_Data.m_Array.m_u8Short[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_INT16_SHORT:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( int32 )m_Data.m_Array.m_i16Short[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_UINT32:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( int32 )m_Data.m_Array.m_u32[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_UINT64:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( int32 )m_Data.m_Array.m_u64[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_FLOAT32:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( int32 )m_Data.m_Array.m_f32[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_FLOAT64:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( int32 )m_Data.m_Array.m_f64[ i ];
				break;
			}
			default:
				break;
		}
	}

	if ( src_size < dest_size )
		memset( &data[ src_size ], 0, ( dest_size - src_size ) * sizeof( int32 ) );

	return ( src_size == dest_size );
}

bool KeyValues3::ReadArrayFloat32( int dest_size, float32* data ) const
{
	int src_size = 0;

	if ( GetType() == KV3_TYPE_STRING )
	{
		CSplitString values( GetString(), " " );
		src_size = values.Count();
		int count = MIN( src_size, dest_size );
		for ( int i = 0; i < count; ++i )
			data[ i ] = ( float32 )V_StringToFloat64( values[ i ], 0, NULL, NULL, PARSING_FLAG_SKIP_ASSERT );
	}
	else
	{
		switch ( GetTypeEx() )
		{
			case KV3_TYPEEX_ARRAY:
			{
				src_size = m_Data.m_pArray->Count();
				int count = MIN( src_size, dest_size );
				KeyValues3** arr = m_Data.m_pArray->Base();
				for ( int i = 0; i < count; ++i )
					data[ i ] = arr[ i ]->GetFloat();
				break;
			}
			case KV3_TYPEEX_ARRAY_FLOAT32:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				memcpy( data, m_Data.m_Array.m_f32, count * sizeof( float32 ) );
				break;
			}
			case KV3_TYPEEX_ARRAY_FLOAT64:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( float32 )m_Data.m_Array.m_f64[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_INT16:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( float32 )m_Data.m_Array.m_i16[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_INT32:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( float32 )m_Data.m_Array.m_i32[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_UINT8_SHORT:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( float32 )m_Data.m_Array.m_u8Short[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_INT16_SHORT:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( float32 )m_Data.m_Array.m_i16Short[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_UINT32:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( float32 )( float64 )m_Data.m_Array.m_u32[ i ];
				break;
			}
			case KV3_TYPEEX_ARRAY_UINT64:
			{
				src_size = m_nNumArrayElements;
				int count = MIN( src_size, dest_size );
				for ( int i = 0; i < count; ++i )
					data[ i ] = ( float32 )( float64 )m_Data.m_Array.m_u64[ i ];
				break;
			}
			default:
				break;
		}
	}

	if ( src_size < dest_size )
		memset( &data[ src_size ], 0, ( dest_size - src_size ) * sizeof( float32 ) ); 

	return ( src_size == dest_size );
}

int KeyValues3::GetMemberCount() const
{
	if ( GetType() != KV3_TYPE_TABLE )
		return 0;
	
	return m_Data.m_pTable->GetMemberCount();
}

CKeyValues3Table *KeyValues3::GetTable()
{
	if(GetType() != KV3_TYPE_TABLE)
		return nullptr;

	return m_Data.m_pTable;
}

bool KeyValues3::HasInvalidMemberNames() const
{
	if ( GetType() != KV3_TYPE_TABLE )
		return false;

	return m_Data.m_pTable->HasInvalidMemberNames();
}

void KeyValues3::SetHasInvalidMemberNames( bool state )
{
	if ( GetType() == KV3_TYPE_TABLE )
		m_Data.m_pTable->SetHasInvalidMemberNames( state );
}

KeyValues3* KeyValues3::GetMember( KV3MemberId_t id )
{
	if ( GetType() != KV3_TYPE_TABLE || id < 0 || id >= m_Data.m_pTable->GetMemberCount() )
		return nullptr;
	
	return m_Data.m_pTable->GetMember( id );
}

const char* KeyValues3::GetMemberName( KV3MemberId_t id ) const
{
	if ( GetType() != KV3_TYPE_TABLE || id < 0 || id >= m_Data.m_pTable->GetMemberCount() )
		return nullptr;
	
	return m_Data.m_pTable->GetMemberName( this, id );
}

CKV3MemberName KeyValues3::GetKV3MemberName( KV3MemberId_t id ) const
{
	if ( GetType() != KV3_TYPE_TABLE || id < 0 || id >= m_Data.m_pTable->GetMemberCount() )
		return CKV3MemberName();

	return m_Data.m_pTable->GetKV3MemberName( this, id );
}

CUtlStringToken KeyValues3::GetMemberHash( KV3MemberId_t id ) const
{
	if ( GetType() != KV3_TYPE_TABLE || id < 0 || id >= m_Data.m_pTable->GetMemberCount() )
		return CUtlStringToken();
	
	return m_Data.m_pTable->GetMemberHash( id );
}

KeyValues3* KeyValues3::FindMember( const CKV3MemberName &name, KeyValues3* defaultValue )
{
	if ( GetType() != KV3_TYPE_TABLE )
		return defaultValue;

	KV3MemberId_t id = m_Data.m_pTable->FindMember( name );

	if ( id == KV3_INVALID_MEMBER )
		return defaultValue;

	return m_Data.m_pTable->GetMember( id );
}

KeyValues3* KeyValues3::FindMember( const CKV3MemberName &name, KV3MemberId_t &hint, KeyValues3* defaultValue )
{
	if ( GetType() != KV3_TYPE_TABLE )
		return defaultValue;

	KV3MemberId_t id = m_Data.m_pTable->FindMember( name, hint );

	if ( id == KV3_INVALID_MEMBER )
		return defaultValue;

	return m_Data.m_pTable->GetMember( id );
}

KeyValues3* KeyValues3::FindOrCreateMember( const CKV3MemberName &name, bool *pCreated )
{
	if ( GetType() != KV3_TYPE_TABLE )
		PrepareForType( KV3_TYPEEX_TABLE, KV3_SUBTYPE_TABLE );

	KV3MemberId_t id = m_Data.m_pTable->FindMember( name );

	if ( id == KV3_INVALID_MEMBER )
	{
		if ( pCreated )
			*pCreated = true;

		id = m_Data.m_pTable->CreateMember( this, name );
	}
	else
	{
		if ( pCreated )
			*pCreated = false;
	}

	return m_Data.m_pTable->GetMember( id );
}

void KeyValues3::SetToEmptyTable()
{
	PrepareForType( KV3_TYPEEX_TABLE, KV3_SUBTYPE_TABLE );
	m_Data.m_pTable->RemoveAll( this );
}

bool KeyValues3::RemoveMember( KV3MemberId_t id )
{
	if ( GetType() != KV3_TYPE_TABLE || id < 0 || id >= m_Data.m_pTable->GetMemberCount() )
		return false;

	m_Data.m_pTable->RemoveMember( this, id );

	return true;
}

bool KeyValues3::RemoveMember( const KeyValues3* kv )
{
	if ( GetType() != KV3_TYPE_TABLE )
		return false;

	KV3MemberId_t id = m_Data.m_pTable->FindMember( kv );

	if ( id == KV3_INVALID_MEMBER )
		return false;

	m_Data.m_pTable->RemoveMember( this, id );

	return true;
}

bool KeyValues3::RemoveMember( const CKV3MemberName &name )
{
	if ( GetType() != KV3_TYPE_TABLE )
		return false;

	KV3MemberId_t id = m_Data.m_pTable->FindMember( name );

	if ( id == KV3_INVALID_MEMBER )
		return false;

	m_Data.m_pTable->RemoveMember( this, id );

	return true;
}

KeyValues3* KeyValues3::RenameMember( const CKV3MemberName &name, const CKV3MemberName &newName )
{
	if ( GetType() != KV3_TYPE_TABLE || !newName.GetString() || !newName.GetString()[0] )
		return nullptr;

	if ( m_Data.m_pTable->FindMember( newName ) != KV3_INVALID_MEMBER )
		return nullptr;

	KV3MemberId_t id = m_Data.m_pTable->FindMember( name );

	if ( id == KV3_INVALID_MEMBER )
		return nullptr;

	m_Data.m_pTable->RenameMember( this, id, newName );

	return m_Data.m_pTable->GetMember( id );
}

void KeyValues3::OverlayKeysFrom( const KeyValues3 *src, bool bRecursive )
{
	if ( GetType() != KV3_TYPE_TABLE )
		PrepareForType( KV3_TYPEEX_TABLE, KV3_SUBTYPE_TABLE );

	for ( int i = 0; i < src->GetMemberCount(); ++i )
	{
		const KeyValues3 *src_member = src->GetMember( i );

		// Without the symbol id, which is only valid in the arena that made it
		KeyValues3 *member = FindOrCreateMember( CKV3MemberName( src->GetMemberHash( i ), src->GetMemberName( i ) ) );

		if ( bRecursive && member->IsTable() && src_member->IsTable() )
			member->OverlayKeysFrom( src_member, true );
		else
			member->CopyFrom( src_member );
	}
}

const char* KeyValues3::GetTypeAsString() const
{
	static const char* s_Types[] =
	{
		"invalid",
		"null",
		"bool",
		"int",
		"uint",
		"double",
		"string",
		"binary_blob",
		"array",
		"table",
		nullptr
	};

	KV3Type_t type = GetType();

	if ( type < KV3_TYPE_COUNT )
		return s_Types[type];

	return "<unknown>";
}

const char* KeyValues3::GetSubTypeAsString() const
{
	static const char* s_SubTypes[] =
	{
		"invalid",
		"resource",
		"resource_name",
		"panorama",
		"soundevent",
		"subclass",
		"entity_name",
		"localize",
		"unspecified",
		"null",
		"binary_blob",
		"array",
		"table",
		"bool8",
		"char8",
		"uchar32",
		"int8",
		"uint8",
		"int16",
		"uint16",
		"int32",
		"uint32",
		"int64",
		"uint64",
		"float32",
		"float64",
		"string",
		"pointer",
		"color32",
		"vector",
		"vector2d",
		"vector4d",
		"rotation_vector",
		"quaternion",
		"qangle",
		"matrix3x4",
		"transform",
		"string_token",
		"ehandle",
		nullptr
	};

	KV3SubType_t subtype = GetSubType();

	if ( subtype < KV3_SUBTYPE_COUNT )
		return s_SubTypes[subtype];

	return "<unknown>";
}

const char* KeyValues3::ToString( CBufferString& buff, uint flags ) const
{
	if ( ( flags & KV3_TO_STRING_DONT_CLEAR_BUFF ) != 0 )
		flags &= ~KV3_TO_STRING_DONT_APPEND_STRINGS;
	else
		buff.Clear();

	KV3Type_t type = GetType();

	switch ( type )
	{
		case KV3_TYPE_NULL:
		{
			return buff.Get();
		}
		case KV3_TYPE_BOOL:
		{
			const char* str = m_Data.m_Bool ? "true" : "false";

			if ( ( flags & KV3_TO_STRING_DONT_APPEND_STRINGS ) != 0 )
				return str;

			buff.Insert( buff.Length(), str );
			return buff.Get();
		}
		case KV3_TYPE_INT:
		{
			buff.AppendFormat( "%lld", m_Data.m_Int );
			return buff.Get();
		}
		case KV3_TYPE_UINT:
		{
			if ( GetSubType() == KV3_SUBTYPE_POINTER )
			{
				if ( ( flags & KV3_TO_STRING_APPEND_ONLY_NUMERICS ) == 0 )
					buff.Insert( buff.Length(), "<pointer>" );

				if ( ( flags & KV3_TO_STRING_RETURN_NON_NUMERICS ) == 0 )
					return nullptr;

				return buff.Get();
			}
			
			buff.AppendFormat( "%llu", m_Data.m_UInt );
			return buff.Get();
		}
		case KV3_TYPE_DOUBLE:
		{
			buff.AppendFormat( "%g", m_Data.m_Double );
			return buff.Get();
		}
		case KV3_TYPE_STRING:
		{
			const char* str = GetString();

			if ( ( flags & KV3_TO_STRING_DONT_APPEND_STRINGS ) != 0 )
				return str;

			buff.Insert( buff.Length(), str );
			return buff.Get();
		}
		case KV3_TYPE_BINARY_BLOB:
		{
			if ( ( flags & KV3_TO_STRING_APPEND_ONLY_NUMERICS ) == 0 )
				buff.AppendFormat( "<binary blob: %u bytes>", GetBinaryBlobSize() );

			if ( ( flags & KV3_TO_STRING_RETURN_NON_NUMERICS ) == 0 )
				return nullptr;

			return buff.Get();
		}
		case KV3_TYPE_ARRAY:
		{
			int elements = GetArrayElementCount();

			if ( elements > 0 && elements <= 4 )
			{
				switch ( GetTypeEx() )
				{
					case KV3_TYPEEX_ARRAY:
					{
						bool unprintable = false;
						CBufferStringN<128> temp;

						CKeyValues3Array::Element_t* arr = m_Data.m_pArray->Base();
						for ( int i = 0; i < elements; ++i )
						{
							switch ( arr[i]->GetType() )
							{
								case KV3_TYPE_INT:
									temp.AppendFormat( "%lld", arr[i]->m_Data.m_Int );
									break;
								case KV3_TYPE_UINT:
									if ( arr[i]->GetSubType() == KV3_SUBTYPE_POINTER )
										unprintable = true;
									else
										temp.AppendFormat( "%llu", arr[i]->m_Data.m_UInt );
									break;
								case KV3_TYPE_DOUBLE:
									temp.AppendFormat( "%g", arr[i]->m_Data.m_Double );
									break;
								default:
									unprintable = true;
									break;
							}

							if ( unprintable )
								break;

							if ( i != elements - 1 ) temp.Insert( temp.Length(), " " );
						}

						if ( unprintable )
							break;

						buff.Insert( buff.Length(), temp.Get() );
						return buff.Get();
					}
					case KV3_TYPEEX_ARRAY_FLOAT32:
					{
						for ( int i = 0; i < elements; ++i )
						{
							buff.AppendFormat( "%g", m_Data.m_Array.m_f32[i] );
							if ( i != elements - 1 ) buff.Insert( buff.Length(), " " );
						}
						return buff.Get();
					}
					case KV3_TYPEEX_ARRAY_FLOAT64:
					{
						for ( int i = 0; i < elements; ++i )
						{
							buff.AppendFormat( "%g", m_Data.m_Array.m_f64[i] );
							if ( i != elements - 1 ) buff.Insert( buff.Length(), " " );
						}
						return buff.Get();
					}
					case KV3_TYPEEX_ARRAY_INT16:
					{
						for ( int i = 0; i < elements; ++i )
						{
							buff.AppendFormat( "%d", m_Data.m_Array.m_i16[i] );
							if ( i != elements - 1 ) buff.Insert( buff.Length(), " " );
						}
						return buff.Get();
					}
					case KV3_TYPEEX_ARRAY_INT32:
					{
						for ( int i = 0; i < elements; ++i )
						{
							buff.AppendFormat( "%d", m_Data.m_Array.m_i32[i] );
							if ( i != elements - 1 ) buff.Insert( buff.Length(), " " );
						}
						return buff.Get();
					}
					case KV3_TYPEEX_ARRAY_UINT8_SHORT:
					{
						for ( int i = 0; i < elements; ++i )
						{
							buff.AppendFormat( "%u", m_Data.m_Array.m_u8Short[i] );
							if ( i != elements - 1 ) buff.Insert( buff.Length(), " " );
						}
						return buff.Get();
					}
					case KV3_TYPEEX_ARRAY_INT16_SHORT:
					{
						for ( int i = 0; i < elements; ++i )
						{
							buff.AppendFormat( "%d", m_Data.m_Array.m_i16Short[i] );
							if ( i != elements - 1 ) buff.Insert( buff.Length(), " " );
						}
						return buff.Get();
					}
					case KV3_TYPEEX_ARRAY_UINT32:
					{
						for ( int i = 0; i < elements; ++i )
						{
							buff.AppendFormat( "%u", m_Data.m_Array.m_u32[i] );
							if ( i != elements - 1 ) buff.Insert( buff.Length(), " " );
						}
						return buff.Get();
					}
					case KV3_TYPEEX_ARRAY_UINT64:
					{
						for ( int i = 0; i < elements; ++i )
						{
							buff.AppendFormat( "%llu", m_Data.m_Array.m_u64[i] );
							if ( i != elements - 1 ) buff.Insert( buff.Length(), " " );
						}
						return buff.Get();
					}
					default:
						break;
				}
			}

			if ( ( flags & KV3_TO_STRING_APPEND_ONLY_NUMERICS ) == 0 )
				buff.AppendFormat( "<array: %u elements>", elements );

			if ( ( flags & KV3_TO_STRING_RETURN_NON_NUMERICS ) == 0 )
				return nullptr;

			return buff.Get();
		}
		case KV3_TYPE_TABLE:
		{
			if ( ( flags & KV3_TO_STRING_APPEND_ONLY_NUMERICS ) == 0 )
				buff.AppendFormat( "<table: %u members>", GetMemberCount() );

			if ( ( flags & KV3_TO_STRING_RETURN_NON_NUMERICS ) == 0 )
				return nullptr;

			return buff.Get();
		}
		default: 
		{
			if ( ( flags & KV3_TO_STRING_APPEND_ONLY_NUMERICS ) == 0 )
				buff.AppendFormat( "<unknown KV3 basic type '%s' (%d)>", GetTypeAsString(), type );

			if ( ( flags & KV3_TO_STRING_RETURN_NON_NUMERICS ) == 0 )
				return nullptr;

			return buff.Get();
		}
	}
}

void KeyValues3::CopyFrom( const KeyValues3* pSrc )
{
	if(this == pSrc)
		return;

	SetToNull();

	CKV3Arena* context;
	KV3MetaData_t* pDestMetaData = GetMetaData( &context );

	if ( pDestMetaData )
	{
		KV3MetaData_t* pSrcMetaData = pSrc->GetMetaData();

		if ( pSrcMetaData )
			context->CopyMetaData( pDestMetaData, pSrcMetaData );
		else
			pDestMetaData->Clear();
	}

	KV3SubType_t eSrcSubType = pSrc->GetSubType();

	switch ( pSrc->GetType() )
	{
		case KV3_TYPE_BOOL:
			SetBool( pSrc->m_Data.m_Bool );
			break;
		case KV3_TYPE_INT:
			SetValue<int64>( pSrc->m_Data.m_Int, KV3_TYPEEX_INT, eSrcSubType );
			break;
		case KV3_TYPE_UINT:
			SetValue<uint64>( pSrc->m_Data.m_UInt, KV3_TYPEEX_UINT, eSrcSubType );
			break;
		case KV3_TYPE_DOUBLE:
			SetValue<float64>( pSrc->m_Data.m_Double, KV3_TYPEEX_DOUBLE, eSrcSubType );
			break;
		case KV3_TYPE_STRING:
			SetString( pSrc->GetString(), eSrcSubType );
			break;
		case KV3_TYPE_BINARY_BLOB:
			SetToBinaryBlob( pSrc->GetBinaryBlob(), pSrc->GetBinaryBlobSize() );
			break;
		case KV3_TYPE_ARRAY:
		{
			switch ( pSrc->GetTypeEx() )
			{
				case KV3_TYPEEX_ARRAY:
				{
					PrepareForType( KV3_TYPEEX_ARRAY, KV3_SUBTYPE_ARRAY );
					m_Data.m_pArray->CopyFrom( this, pSrc->m_Data.m_pArray );
					break;
				}
				case KV3_TYPEEX_ARRAY_FLOAT32:
					AllocArray<float32>( pSrc->m_nNumArrayElements, pSrc->m_Data.m_Array.m_f32, KV3_ARRAY_ALLOC_NORMAL, KV3_TYPEEX_INVALID, KV3_TYPEEX_ARRAY_FLOAT32, eSrcSubType, KV3_TYPEEX_DOUBLE, KV3_SUBTYPE_FLOAT32 );
					break;
				case KV3_TYPEEX_ARRAY_FLOAT64:
					AllocArray<float64>( pSrc->m_nNumArrayElements, pSrc->m_Data.m_Array.m_f64, KV3_ARRAY_ALLOC_NORMAL, KV3_TYPEEX_INVALID, KV3_TYPEEX_ARRAY_FLOAT64, eSrcSubType, KV3_TYPEEX_DOUBLE, KV3_SUBTYPE_FLOAT64 );
					break;
				case KV3_TYPEEX_ARRAY_INT16:
					AllocArray<int16>( pSrc->m_nNumArrayElements, pSrc->m_Data.m_Array.m_i16, KV3_ARRAY_ALLOC_NORMAL, KV3_TYPEEX_ARRAY_INT16_SHORT, KV3_TYPEEX_ARRAY_INT16, eSrcSubType, KV3_TYPEEX_INT, KV3_SUBTYPE_INT16 );
					break;
				case KV3_TYPEEX_ARRAY_INT32:
					AllocArray<int32>( pSrc->m_nNumArrayElements, pSrc->m_Data.m_Array.m_i32, KV3_ARRAY_ALLOC_NORMAL, KV3_TYPEEX_INVALID, KV3_TYPEEX_ARRAY_INT32, eSrcSubType, KV3_TYPEEX_INT, KV3_SUBTYPE_INT32 );
					break;
				case KV3_TYPEEX_ARRAY_UINT8_SHORT:
					AllocArray<uint8>( pSrc->m_nNumArrayElements, pSrc->m_Data.m_Array.m_u8Short, KV3_ARRAY_ALLOC_NORMAL, KV3_TYPEEX_ARRAY_UINT8_SHORT, KV3_TYPEEX_INVALID, eSrcSubType, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT8 );
					break;
				case KV3_TYPEEX_ARRAY_INT16_SHORT:
					AllocArray<int16>( pSrc->m_nNumArrayElements, pSrc->m_Data.m_Array.m_i16Short, KV3_ARRAY_ALLOC_NORMAL, KV3_TYPEEX_ARRAY_INT16_SHORT, KV3_TYPEEX_ARRAY_INT16, eSrcSubType, KV3_TYPEEX_INT, KV3_SUBTYPE_INT16 );
					break;
				case KV3_TYPEEX_ARRAY_UINT32:
					AllocArray<uint32>( pSrc->m_nNumArrayElements, pSrc->m_Data.m_Array.m_u32, KV3_ARRAY_ALLOC_NORMAL, KV3_TYPEEX_INVALID, KV3_TYPEEX_ARRAY_UINT32, eSrcSubType, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT32 );
					break;
				case KV3_TYPEEX_ARRAY_UINT64:
					AllocArray<uint64>( pSrc->m_nNumArrayElements, pSrc->m_Data.m_Array.m_u64, KV3_ARRAY_ALLOC_NORMAL, KV3_TYPEEX_INVALID, KV3_TYPEEX_ARRAY_UINT64, eSrcSubType, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT64 );
					break;
				default:
					break;
			}
			break;
		}
		case KV3_TYPE_TABLE:
		{
			SetToEmptyTable();
			m_Data.m_pTable->CopyFrom( this, pSrc, pSrc->m_Data.m_pTable );
			break;
		}
		default:
			break;
	}

	m_SubType = eSrcSubType;
	m_nFlags = pSrc->m_nFlags;
}

KeyValues3& KeyValues3::operator=( const KeyValues3& src )
{
	if ( this == &src )
		return *this;

	CopyFrom( &src );

	return *this;
}

void CKeyValues3Iterator::Init( KeyValues3 *kv )
{
	m_Stack.Purge();

	if(kv)
	{
		auto entry = m_Stack.AddToTailGetPtr();
		entry->m_nIndex = -1;
		entry->m_pKV = kv;
	}
}

void CKeyValues3Iterator::Advance()
{
	while(m_Stack.Count() > 0)
	{
		auto &entry = m_Stack[m_Stack.Count() - 1];
		auto kv = entry.m_pKV;

		if(kv->GetType() == KV3_TYPE_ARRAY)
		{
			entry.m_nIndex++;

			if(entry.m_nIndex < kv->GetArrayElementCount())
			{
				auto new_entry = m_Stack.AddToTailGetPtr();
				new_entry->m_nIndex = -1;
				new_entry->m_pKV = kv->GetArrayElement( entry.m_nIndex );
				return;
			}
		}
		else if(kv->GetType() == KV3_TYPE_TABLE)
		{
			entry.m_nIndex++;

			if(entry.m_nIndex < kv->GetMemberCount())
			{
				auto new_entry = m_Stack.AddToTailGetPtr();
				new_entry->m_nIndex = -1;
				new_entry->m_pKV = kv->GetMember( entry.m_nIndex );
				return;
			}
		}

		m_Stack.RemoveMultipleFromTail( 1 );
	}
}

CKeyValues3Array::CKeyValues3Array( int cluster_elem, int alloc_size ) :
	m_nClusterElement( cluster_elem ),
	m_nAllocatedChunks( alloc_size ),
	m_nCount( 0 ),
	m_nInitialSize( MIN( alloc_size, 255 ) ),
	m_bIsDynamicallySized( false ),
	m_pDynamicElements( nullptr )
{
}

CKeyValues3ArrayCluster* CKeyValues3Array::GetCluster() const
{
	if ( m_nClusterElement == -1 )
		return nullptr;

	return GET_OUTER( CKeyValues3ArrayCluster, m_Values[ m_nClusterElement ] );
}

CKV3Arena* CKeyValues3Array::GetContext() const
{ 
	CKeyValues3ArrayCluster* cluster = GetCluster();

	if ( cluster )
		return cluster->GetContext();
	else
		return nullptr;
}

KeyValues3* CKeyValues3Array::Element( int i )
{
	Assert( 0 <= i && i < m_nCount );

	return Base()[i];
}

void CKeyValues3Array::EnsureElementCapacity( int count, bool force, bool dont_move )
{
	if(count <= m_nAllocatedChunks)
		return;

	if(count > ALLOC_KV3ARRAY_MAX)
	{
		Plat_FatalError( "%s: element count overflow (%u)\n", __FUNCTION__, count );
		DebuggerBreak();
	}

	const int new_count = force ? count : CalcNewKV3Capacity( m_nAllocatedChunks, count, ALLOC_KV3ARRAY_MIN, ALLOC_KV3ARRAY_MAX );
	const int new_byte_size = (int)TotalSizeOfData( new_count );

	Element_t *new_base = nullptr;

	if(m_bIsDynamicallySized)
	{
		new_base = (Element_t *)realloc( m_pDynamicElements, new_byte_size );
	}
	else
	{
		new_base = (Element_t *)malloc( new_byte_size );

		if(m_nCount > 0 && !dont_move)
		{
			memmove( new_base, Base(), sizeof( Element_t ) * m_nCount );
		}
	}

	m_pDynamicElements = new_base;
	m_nAllocatedChunks = new_count;
	m_bIsDynamicallySized = true;
}

void CKeyValues3Array::SetCount( KeyValues3 *parent, int count, KV3TypeEx_t type, KV3SubType_t subtype )
{
	Element_t *elements_base = Base();
	for(int i = count; i < m_nCount; i++)
	{
		parent->FreeMember( elements_base[i] );
	}

	EnsureElementCapacity( count );

	elements_base = Base();
	for(int i = m_nCount; i < count; i++)
	{
		elements_base[i] = parent->AllocMember( type, subtype );
	}

	m_nCount = count;
}

CKeyValues3Array::Element_t* CKeyValues3Array::InsertMultipleBefore( KeyValues3 *parent, int from, int num )
{
	if(from < 0 || from > m_nCount)
	{
		Plat_FatalError( "%s: invalid insert point %u (current count %u)\n", __FUNCTION__, from, m_nCount );
		DebuggerBreak();
	}

	if(num > ALLOC_KV3ARRAY_MAX - m_nCount)
	{
		Plat_FatalError( "%s: max element overflow, cur count %u + %u\n", __FUNCTION__, m_nCount, num );
		DebuggerBreak();
	}

	int new_size = m_nCount + num;
	EnsureElementCapacity( new_size );

	Element_t *base = Base();
	if(from < m_nCount)
	{
		memmove( &base[from + num], &base[from], sizeof(Element_t) * (m_nCount - from) );
	}

	for ( int i = 0; i < num; ++i )
	{
		base[from + i] = parent->AllocMember();
	}

	m_nCount = new_size;

	return &base[from];
}

void CKeyValues3Array::CopyFrom( KeyValues3 *parent, const CKeyValues3Array* pSrc )
{
	int nNewSize = pSrc->Count();

	SetCount( parent, nNewSize );

	Element_t *base = Base();
	Element_t const *pSrcKV = pSrc->Base();

	for ( int i = 0; i < nNewSize; ++i )
		*base[i] = *pSrcKV[i];
}

void CKeyValues3Array::RemoveMultiple( KeyValues3 *parent, int from, int num )
{
	if(num > m_nCount)
	{
		Plat_FatalError( "%s: invalid array remove count %u (current count %u)\n", __FUNCTION__, num, m_nCount );
		DebuggerBreak();
	}

	if(from < 0 || from > m_nCount - num)
	{
		Plat_FatalError( "%s: invalid array remove point %u (current count %u)\n", __FUNCTION__, from, m_nCount );
		DebuggerBreak();
	}

	Element_t *base = Base();

	for ( int i = 0; i < num; ++i )
	{
		parent->FreeMember( base[from + i] );
	}

	if ( from + num < m_nCount )
	{
		memmove( &base[from], &base[from + num], sizeof(Element_t) * (m_nCount - from - num) );
	}

	m_nCount -= num;
}

void CKeyValues3Array::PurgeBuffers()
{
	if(m_bIsDynamicallySized)
	{
		free( m_pDynamicElements );
		m_nAllocatedChunks = m_nInitialSize;
		m_bIsDynamicallySized = false;
	}

	m_nCount = 0;
}

void CKeyValues3Array::PurgeContent( KeyValues3 *parent, bool clearing_context )
{
	if(!clearing_context && parent)
	{
		auto elements_base = Base();

		for(int i = 0; i < m_nCount; i++)
		{
			parent->FreeMember( elements_base[i] );
		}
	}
}

CKeyValues3Table::CKeyValues3Table( int cluster_elem, int alloc_size ) :
	m_nClusterElement( cluster_elem ),
	m_nAllocatedChunks( alloc_size ),
	m_pFastSearch( nullptr ),
	m_nCount( 0 ),
	m_nInitialSize( MIN( alloc_size, 255 ) ),
	m_bIsDynamicallySized( false ),
	m_bHasInvalidMemberNames( false ),
	m_unk001( false ),
	m_pDynamicBuffer( nullptr )
{
}

CKeyValues3TableCluster* CKeyValues3Table::GetCluster() const
{
	if ( m_nClusterElement == -1 )
		return nullptr;

	return GET_OUTER( CKeyValues3TableCluster, m_Values[ m_nClusterElement ] );
}

CKV3Arena* CKeyValues3Table::GetContext() const
{ 
	CKeyValues3TableCluster* cluster = GetCluster();

	if ( cluster )
		return cluster->GetContext();
	else
		return nullptr;
}

KeyValues3* CKeyValues3Table::GetMember( KV3MemberId_t id )
{
	Assert( 0 <= id && id < m_nCount );

	return MembersBase()[id];
}

const char* CKeyValues3Table::GetMemberName( const KeyValues3 *parent, KV3MemberId_t id ) const
{
	Assert( 0 <= id && id < m_nCount );

	Flags_t flags = FlagsBase()[id];

	if((flags & MEMBER_FLAG_LARGE_SYMBOL) != 0)
	{
		UtlSymLargeId_t symid = NamesBase()[id].m_SymId;
		return parent->GetContext()->LookupString( symid );
	}

	return NamesBase()[id].m_String;
}

const CKeyValues3Table::Hash_t CKeyValues3Table::GetMemberHash( KV3MemberId_t id ) const
{
	Assert( 0 <= id && id < m_nCount );

	return HashesBase()[id];
}

CKV3MemberName CKeyValues3Table::GetKV3MemberName( const KeyValues3 *parent, KV3MemberId_t id ) const
{
	Assert( 0 <= id && id < m_nCount );

	Flags_t flags = FlagsBase()[id];

	UtlSymLargeId_t symid = UTL_INVAL_SYMBOL_LARGE;
	const char *key_name = nullptr;
	if((flags & MEMBER_FLAG_LARGE_SYMBOL) != 0)
	{
		symid = NamesBase()[id].m_SymId;
		key_name = parent->GetContext()->LookupString( symid );
	}
	else
	{
		key_name = NamesBase()[id].m_String;
	}

	return CKV3MemberName( GetMemberHash( id ), key_name, symid );
}

void CKeyValues3Table::EnableFastSearch()
{
	if ( m_pFastSearch )
		m_pFastSearch->m_member_ids.RemoveAll();
	else
		m_pFastSearch = new kv3tablefastsearch_t;

	const Hash_t* pHashes = HashesBase();

	for ( int i = 0; i < m_nCount; ++i )
	{
		m_pFastSearch->m_member_ids.Insert( pHashes[i].GetHashCode(), i );
	}

	m_pFastSearch->m_ignore = false;
	m_pFastSearch->m_ignores_counter = 0;
}

void CKeyValues3Table::EnsureMemberCapacity( int count, bool force, bool dont_move )
{
	if(count <= m_nAllocatedChunks)
		return;

	if(count > ALLOC_KV3TABLE_MAX)
	{
		Plat_FatalError( "%s member count overflow (%u)\n", __FUNCTION__, count );
		DebuggerBreak();
	}

	const int new_count = force ? count : CalcNewKV3Capacity( m_nAllocatedChunks, count, ALLOC_KV3TABLE_MIN, ALLOC_KV3TABLE_MAX );
	const int new_byte_size = (int)TotalSizeOfData( new_count );

	if(m_bIsDynamicallySized)
	{
		m_pDynamicBuffer = realloc( m_pDynamicBuffer, new_byte_size );

		if(m_nCount > 0 && !dont_move)
		{
			memmove( (uint8 *)m_pDynamicBuffer + OffsetToFlagsBase( new_count ), FlagsBase(), m_nCount * sizeof( Flags_t ) );
			memmove( (uint8 *)m_pDynamicBuffer + OffsetToNamesBase( new_count ), NamesBase(), m_nCount * sizeof( Name_t ) );
			memmove( (uint8 *)m_pDynamicBuffer + OffsetToMembersBase( new_count ), MembersBase(), m_nCount * sizeof( Member_t ) );
		}
	}
	else
	{
		void *new_base = malloc( new_byte_size );

		if(m_nCount > 0 && !dont_move)
		{
			memmove( (uint8 *)new_base + OffsetToHashesBase( new_count ), HashesBase(), m_nCount * sizeof( Hash_t ) );
			memmove( (uint8 *)new_base + OffsetToMembersBase( new_count ), MembersBase(), m_nCount * sizeof( Member_t ) );
			memmove( (uint8 *)new_base + OffsetToNamesBase( new_count ), NamesBase(), m_nCount * sizeof( Name_t ) );
			memmove( (uint8 *)new_base + OffsetToFlagsBase( new_count ), FlagsBase(), m_nCount * sizeof( Flags_t ) );
		}

		m_pDynamicBuffer = new_base;
		m_bIsDynamicallySized = true;
	}

	m_nAllocatedChunks = new_count;
}

KV3MemberId_t CKeyValues3Table::FindMember( const KeyValues3* kv ) const
{
	const Member_t* pMembers = MembersBase();

	for ( int i = 0; i < m_nCount; ++i )
	{
		if ( pMembers[i] == kv )
			return i;
	}

	return KV3_INVALID_MEMBER;
}

KV3MemberId_t CKeyValues3Table::FindMember( const CKV3MemberName &name )
{
	KV3MemberId_t hint = KV3_INVALID_MEMBER;
	return FindMember( name, hint );
}

KV3MemberId_t CKeyValues3Table::FindMember( const CKV3MemberName &name, KV3MemberId_t &hint )
{
	bool bFastSearch = false;

	if ( m_pFastSearch )
	{
		if ( m_pFastSearch->m_ignore )
		{
			if ( ++m_pFastSearch->m_ignores_counter > 4 )
			{
				EnableFastSearch();
				bFastSearch = true;
			}
		}
		else
		{
			bFastSearch = true;
		}
	}

	if ( bFastSearch )
	{
		UtlHashHandle_t h = m_pFastSearch->m_member_ids.Find( name.GetHashCode() );

		if ( h != m_pFastSearch->m_member_ids.InvalidHandle() )
			return m_pFastSearch->m_member_ids[ h ];
	}
	else
	{
		const Hash_t* pHashes = HashesBase();

		if ( (uint32)hint < (uint32)m_nCount && pHashes[hint] == name.GetHashCode() )
			return hint++;

		for ( int i = 0; i < m_nCount; ++i )
		{
			if ( pHashes[i] == name.GetHashCode() )
			{
				hint = i + 1;
				return i;
			}
		}
	}

	return KV3_INVALID_MEMBER;
}

KV3MemberId_t CKeyValues3Table::CreateMember( KeyValues3 *parent, const CKV3MemberName &name, bool name_external )
{
	if ( GetMemberCount() >= 128 && !m_pFastSearch )
		EnableFastSearch();

	KV3MemberId_t curr = m_nCount;

	int new_size = m_nCount + 1;
	EnsureMemberCapacity( new_size );

	Hash_t *hashes_base = HashesBase();
	Member_t *members_base = MembersBase();
	Name_t *names_base = NamesBase();
	Flags_t *flags_base = FlagsBase();

	members_base[curr] = parent->AllocMember();
	hashes_base[curr] = name.GetHashCode();
	
	StoreKeyName( parent, names_base[curr], flags_base[curr], name.GetString(), name.GetSymId(), name_external );

	if ( m_pFastSearch && !m_pFastSearch->m_ignore )
		m_pFastSearch->m_member_ids.Insert( name.GetHashCode(), curr );

	m_nCount = new_size;

	return curr;
}

void CKeyValues3Table::StoreKeyName( KeyValues3 *parent, Name_t &out_buffer, Flags_t &out_flags, const char *input_string, UtlSymLargeId_t sym_id, bool name_external )
{
	Flags_t flags = 0;

	if(name_external)
	{
		out_buffer.m_String = input_string;
		flags |= MEMBER_FLAG_EXTERNAL_NAME;
	}
	else
	{
		auto context = parent->GetContext();

		if(context)
		{
			const char *existing_str = context->LookupString( sym_id );

			if(existing_str && (existing_str == input_string || strcmp(existing_str, input_string) == 0))
			{
				out_buffer.m_SymId = sym_id;
				flags |= MEMBER_FLAG_LARGE_SYMBOL;
			}
			else
			{
				const char *alloced_string = context->AllocString( input_string, &sym_id );

				if(sym_id != UTL_INVAL_SYMBOL_LARGE)
				{
					out_buffer.m_SymId = sym_id;
					flags |= MEMBER_FLAG_LARGE_SYMBOL;
				}
				else
				{
					out_buffer.m_String = alloced_string;
				}
			}
		}
		else
		{
			out_buffer.m_String = strdup( input_string );
		}
	}

	out_flags = flags;
}

void CKeyValues3Table::CopyFrom( KeyValues3 *parent, const KeyValues3 *src_parent, const CKeyValues3Table* src )
{
	int new_size = src->GetMemberCount();

	RemoveAll( parent, new_size );
	EnsureMemberCapacity( new_size, true, true );

	auto context = parent->GetContext();
	auto src_context = src_parent->GetContext();

	Hash_t *hashes_base = HashesBase();
	Member_t *members_base = MembersBase();
	Name_t *names_base = NamesBase();
	Flags_t *flags_base = FlagsBase();

	const Hash_t *src_hashes_base = src->HashesBase();
	const Member_t *src_members_base = src->MembersBase();
	const Name_t *src_names_base = src->NamesBase();
	const Flags_t *src_flags_base = src->FlagsBase();

	memmove( hashes_base, src_hashes_base, sizeof(Hash_t) * new_size );

	for(int i = 0; i < new_size; i++)
	{
		if((src_flags_base[i] & MEMBER_FLAG_LARGE_SYMBOL) != 0)
		{
			// Symbol ids are only valid in the arena that made them
			UtlSymLargeId_t sym_id = src_names_base[i].m_SymId;
			StoreKeyName( parent, names_base[i], flags_base[i], src_context->LookupString( sym_id ), context == src_context ? sym_id : UTL_INVAL_SYMBOL_LARGE );
		}
		else
			StoreKeyName( parent, names_base[i], flags_base[i], src_names_base[i].m_String );

		members_base[i] = parent->AllocMember();
		members_base[i]->CopyFrom( src_members_base[i] );
	}

	m_nCount = new_size;

	if ( new_size >= 128 )
		EnableFastSearch();
}

void CKeyValues3Table::PurgeNameBuffer( KeyValues3 *parent, KV3MemberId_t id )
{
	auto name = NamesBase()[id];
	auto flags = FlagsBase()[id];

	if((flags & MEMBER_FLAG_EXTERNAL_NAME) == 0 &&
		(flags & MEMBER_FLAG_LARGE_SYMBOL) == 0 &&
		!parent->GetContext() && name.m_String)
	{
		free( (void *)name.m_String );
	}
}

void CKeyValues3Table::RemoveMember( KeyValues3 *parent, KV3MemberId_t id )
{
	m_nCount--;

	Hash_t* hashes_base = HashesBase();
	Member_t* members_base = MembersBase();
	Name_t* names_base = NamesBase();
	Flags_t* flags_base = FlagsBase();

	parent->FreeMember( members_base[id] );

	PurgeNameBuffer( parent, id );

	if ( id < m_nCount )
	{
		int shift_size = m_nCount - id;
		int shift_from = id + 1;

		memmove( &hashes_base[id], &hashes_base[shift_from], shift_size * sizeof(Hash_t) );
		memmove( &members_base[id], &members_base[shift_from], shift_size * sizeof(Member_t) );
		memmove( &names_base[id], &names_base[shift_from], shift_size * sizeof(Name_t) );
		memmove( &flags_base[id], &flags_base[shift_from], shift_size * sizeof(Flags_t) );
	}

	if ( m_pFastSearch )
	{
		m_pFastSearch->m_ignore = true;
		m_pFastSearch->m_ignores_counter = 1;
	}
}

void CKeyValues3Table::RenameMember( KeyValues3 *parent, KV3MemberId_t id, const CKV3MemberName &newName )
{
	PurgeNameBuffer( parent, id );

	HashesBase()[id] = newName.GetHashCode();
	StoreKeyName( parent, NamesBase()[id], FlagsBase()[id], newName.GetString(), newName.GetSymId() );

	if ( m_pFastSearch )
	{
		m_pFastSearch->m_ignore = true;
		m_pFastSearch->m_ignores_counter = 1;
	}
}

void CKeyValues3Table::RemoveAll( KeyValues3 *parent, int new_size )
{
	Member_t *members_base = MembersBase();

	for(int i = 0; i < m_nCount; i++)
	{
		parent->FreeMember( members_base[i] );
		PurgeNameBuffer( parent, i );
	}

	m_nCount = 0;
	if(new_size > 0)
	{
		EnsureMemberCapacity( new_size, true, true );
	}

	if(new_size < 128)
	{
		PurgeFastSearch();
	}
	else
	{
		EnableFastSearch();
		m_pFastSearch->m_member_ids.Reserve( new_size );
	}
}

void CKeyValues3Table::PurgeFastSearch()
{
	if(m_pFastSearch)
		delete m_pFastSearch;

	m_pFastSearch = nullptr;
}

void CKeyValues3Table::PurgeContent( KeyValues3 *parent, bool bClearingContext )
{
	Member_t *members_base = MembersBase();

	for ( int i = 0; i < m_nCount; ++i )
	{
		if(!bClearingContext && parent)
		{
			parent->FreeMember( members_base[i] );
		}

		PurgeNameBuffer( parent, i );
	}

	m_nCount = 0;

	PurgeFastSearch();
}

void CKeyValues3Table::PurgeBuffers()
{
	if(m_bIsDynamicallySized)
	{
		free( m_pDynamicBuffer );
		m_nAllocatedChunks = m_nInitialSize;
		m_bIsDynamicallySized = false;
	}

	m_nCount = 0;
}

CKV3ArenaBase::CKV3ArenaBase( CKV3Arena* context ) :
	m_pContext( context ),
	m_KV3BaseCluster( context ),
	m_bMetaDataEnabled( false ),
	m_bFormatConverted( false ),
	m_bRootAvailabe( false ),
	m_pParsingErrorListener( nullptr )
{
	m_KV3PartialClusters.AddToChain( &m_KV3BaseCluster );
}

void CKV3ArenaBase::Clear()
{
	m_BinaryData.Clear();
	m_KV3BaseCluster.Clear();
	m_Symbols.RemoveAll();
	
	m_bFormatConverted = false;
}

void CKV3ArenaBase::Purge()
{
	m_BinaryData.Purge();
	m_KV3BaseCluster.Purge();
	m_Symbols.Purge();

	m_bFormatConverted = false;
}

CKV3Arena::CKV3Arena( bool bNoRoot ) : BaseClass( this ), pad{}
{
	if ( bNoRoot )
	{
		m_bRootAvailabe =  false;
	}
	else
	{
		m_bRootAvailabe = true;
		m_KV3BaseCluster.Alloc();
	}

	m_bMetaDataEnabled = false;
	m_bFormatConverted = false;
}

void CKV3Arena::Clear()
{
	BaseClass::Clear();

	ClearClusterNodeChain( m_KV3PartialClusters );
	ClearClusterNodeChain( m_KV3FullClusters );
	MoveToPartial( m_KV3FullClusters, m_KV3PartialClusters );

	ClearClusterNodeChain( m_PartialArrayClusters );
	ClearClusterNodeChain( m_FullArrayClusters );
	MoveToPartial( m_FullArrayClusters, m_PartialArrayClusters );
	m_RawArrayEntries.Clear();

	ClearClusterNodeChain( m_PartialTableClusters );
	ClearClusterNodeChain( m_FullTableClusters );
	MoveToPartial( m_FullTableClusters, m_PartialTableClusters );
	m_RawTableEntries.Clear();

	if ( m_bRootAvailabe )
		m_KV3BaseCluster.Alloc();
}

void CKV3Arena::Purge()
{
	BaseClass::Purge();

	PurgeClusterNodeChain( m_KV3PartialClusters );
	PurgeClusterNodeChain( m_KV3FullClusters );
	m_KV3PartialClusters.AddToChain( &m_KV3BaseCluster );
	m_KV3BaseCluster.EnableMetaData( m_bMetaDataEnabled );

	PurgeClusterNodeChain( m_PartialArrayClusters );
	PurgeClusterNodeChain( m_FullArrayClusters );
	m_RawArrayEntries.Purge();

	PurgeClusterNodeChain( m_PartialTableClusters );
	PurgeClusterNodeChain( m_FullTableClusters );
	m_RawTableEntries.Purge();

	if ( m_bRootAvailabe )
		m_KV3BaseCluster.Alloc();
}

KeyValues3* CKV3Arena::Root()
{
	if ( !m_bRootAvailabe )
	{
		Plat_FatalError( "FATAL: %s called on a pool context (no root available)\n", __FUNCTION__ );
		DebuggerBreak();
	}

	return &m_KV3BaseCluster.Head()->m_Value;
}

const char* CKV3Arena::AllocString( const char* pString, UtlSymLargeId_t *out_symid )
{
	UtlSymLargeId_t sym = m_Symbols.AddStringRaw( pString );

	if(out_symid)
		*out_symid = sym;

	if(sym == UTL_INVAL_SYMBOL_LARGE)
		return "";

	return m_Symbols.String( sym );
}

const char* CKV3Arena::LookupString( UtlSymLargeId_t symid ) const
{
	return m_Symbols.String( symid );
}

void CKV3Arena::EnableMetaData( bool bEnable )
{
	if ( bEnable != m_bMetaDataEnabled )
	{
		for ( auto cluster = m_KV3PartialClusters.m_pTail; cluster; cluster = cluster->GetPrev() )
			cluster->EnableMetaData( bEnable );

		for ( auto cluster = m_KV3FullClusters.m_pTail; cluster; cluster = cluster->GetPrev() )
			cluster->EnableMetaData( bEnable );

		m_bMetaDataEnabled = bEnable;
	}
}

void CKV3Arena::CopyMetaData( KV3MetaData_t* pDest, const KV3MetaData_t* pSrc )
{
	pDest->m_nLine = pSrc->m_nLine;
	pDest->m_nColumn = pSrc->m_nColumn;
	pDest->m_nFlags = pSrc->m_nFlags;
	pDest->m_sName = m_Symbols.AddString( pSrc->m_sName.String() );

	pDest->m_Comments.Purge();
	pDest->m_Comments.EnsureCapacity( pSrc->m_Comments.Count() );

	FOR_EACH_MAP_FAST( pSrc->m_Comments, iter )
	{
		pDest->m_Comments.Insert( pSrc->m_Comments.Key( iter ), pSrc->m_Comments.Element( iter ) );
	}
}

KeyValues3* CKV3Arena::AllocKV( KV3TypeEx_t type, KV3SubType_t subtype )
{
	return Alloc( m_KV3PartialClusters, m_KV3FullClusters, CKeyValues3Cluster::CLUSTER_SIZE, type, subtype );
}

void CKV3Arena::FreeKV( KeyValues3* kv )
{
	CKV3Arena* context;
	KV3MetaData_t* metadata = kv->GetMetaData( &context );

	if ( metadata )
		metadata->Clear();

	Free( kv, m_KV3PartialClusters, m_KV3FullClusters );
}

#include "public/tier0/memdbgoff.h"

KV3MetaData_t::KV3MetaData_t()
: m_nLine( 0 ), m_nColumn( 0 ), m_nFlags( 0 )
{}

void KV3MetaData_t::Clear()
{
	m_nLine = 0;
	m_nColumn = 0;
	m_nFlags = 0;
	m_sName = CUtlSymbolLarge();
	m_Comments.RemoveAll();
}

void KV3MetaData_t::Purge()
{
	m_nLine = 0;
	m_nColumn = 0;
	m_nFlags = 0;
	m_sName = CUtlSymbolLarge();
	m_Comments.Purge();
}

CKV3MemberName::CKV3MemberName()
: m_nHashCode(), m_SymId( UTL_INVAL_SYMBOL_LARGE ), m_pszString("")
{}

CKV3MemberName::CKV3MemberName( CUtlStringToken nHashCode, const char* pszString, UtlSymLargeId_t symid )
: m_nHashCode(nHashCode), m_SymId( symid ), m_pszString(pszString)
{}

unsigned int CKV3MemberName::GetHashCode() const
{ return m_nHashCode.GetHashCode(); }

const char* CKV3MemberName::GetString() const
{ return m_pszString; }

UtlSymLargeId_t CKV3MemberName::GetSymId() const
{ return m_SymId; }

CKV3MemberNameWithStorage::CKV3MemberNameWithStorage()
{ Set( "" ); }

CKV3MemberNameWithStorage::CKV3MemberNameWithStorage( const char *pszString )
{ Set( pszString ); }

CKV3MemberNameWithStorage::CKV3MemberNameWithStorage( const CKV3MemberNameWithStorage &other )
{ Set( other.GetString() ); }

CKV3MemberNameWithStorage & CKV3MemberNameWithStorage::operator=( const CKV3MemberNameWithStorage &other )
{
	if ( this != &other )
		Set( other.GetString() );

	return *this;
}

void CKV3MemberNameWithStorage::Set( const char *pszString )
{
	m_Storage.Clear();
	m_Storage.Insert( 0, pszString ? pszString : "" );
	m_nHashCode = CUtlStringToken( pszString );
	m_SymId = UTL_INVAL_SYMBOL_LARGE;
	m_pszString = m_Storage.Get();
}

bool KeyValues3::HasFlag( KeyValues3Flag_t flag ) const
{ return (m_nFlags & flag) != 0; }

bool KeyValues3::HasAnyFlags() const
{ return m_nFlags != 0; }

KeyValues3Flag_t KeyValues3::GetAllFlags() const
{ return (KeyValues3Flag_t)m_nFlags; }

void KeyValues3::SetAllFlags( KeyValues3Flag_t flags )
{ m_nFlags |= flags; }

void KeyValues3::SetFlag( KeyValues3Flag_t flag, bool state )
{
	if(state)
		m_nFlags |= flag;
	else
		m_nFlags &= ~flag;
}

KV3Type_t KeyValues3::GetType() const
{ return ( KV3Type_t )( m_TypeEx & 0xF ); }

KV3TypeEx_t KeyValues3::GetTypeEx() const
{ return ( KV3TypeEx_t )m_TypeEx; }

KV3SubType_t KeyValues3::GetSubType() const
{ return ( KV3SubType_t )m_SubType; }

void KeyValues3::SetToNull()
{ PrepareForType( KV3_TYPEEX_NULL, KV3_SUBTYPE_NULL ); }

bool KeyValues3::IsNull() const
{ return GetType() == KV3_TYPE_NULL; }

bool KeyValues3::IsArray() const
{ return GetType() == KV3_TYPE_ARRAY; }

bool KeyValues3::IsTable() const
{ return GetType() == KV3_TYPE_TABLE; }

bool KeyValues3::GetBool( bool defaultValue ) const
{ return GetValue<bool>( defaultValue ); }

int8_t KeyValues3::GetChar( int8_t defaultValue ) const
{ return GetValue<int8_t>( defaultValue ); }

uint32_t KeyValues3::GetUChar32( uint32_t defaultValue ) const
{ return GetValue<uint32_t>( defaultValue ); }

int8_t KeyValues3::GetInt8( int8_t defaultValue ) const
{ return GetValue<int8_t>( defaultValue ); }

uint8_t KeyValues3::GetUInt8( uint8_t defaultValue ) const
{ return GetValue<uint8_t>( defaultValue ); }

int16_t KeyValues3::GetShort( int16_t defaultValue ) const
{ return GetValue<int16_t>( defaultValue ); }

uint16_t KeyValues3::GetUShort( uint16_t defaultValue ) const
{ return GetValue<uint16_t>( defaultValue ); }

int32_t KeyValues3::GetInt( int32_t defaultValue ) const
{ return GetValue<int32_t>( defaultValue ); }

uint32_t KeyValues3::GetUInt( uint32_t defaultValue ) const
{ return GetValue<uint32_t>( defaultValue ); }

int64_t KeyValues3::GetInt64( int64_t defaultValue ) const
{ return GetValue<int64_t>( defaultValue ); }

uint64_t KeyValues3::GetUInt64( uint64_t defaultValue ) const
{ return GetValue<uint64_t>( defaultValue ); }

float KeyValues3::GetFloat( float defaultValue ) const
{ return GetValue<float>( defaultValue ); }

double KeyValues3::GetDouble( double defaultValue ) const
{ return GetValue<double>( defaultValue ); }

void KeyValues3::SetBool( bool value )
{ SetValue<bool>( value, KV3_TYPEEX_BOOL, KV3_SUBTYPE_BOOL8 ); }

void KeyValues3::SetChar( int8_t value )
{ SetValue<int8_t>( value, KV3_TYPEEX_INT, KV3_SUBTYPE_CHAR8 ); }

void KeyValues3::SetUChar32( uint32_t value )
{ SetValue<uint32_t>( value, KV3_TYPEEX_UINT, KV3_SUBTYPE_UCHAR32 ); }

void KeyValues3::SetInt8( int8_t value )
{ SetValue<int8_t>( value, KV3_TYPEEX_INT, KV3_SUBTYPE_INT8 ); }

void KeyValues3::SetUInt8( uint8_t value )
{ SetValue<uint8_t>( value, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT8 ); }

void KeyValues3::SetShort( int16_t value )
{ SetValue<int16_t>( value, KV3_TYPEEX_INT, KV3_SUBTYPE_INT16 ); }

void KeyValues3::SetUShort( uint16_t value )
{ SetValue<uint16_t>( value, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT16 ); }

void KeyValues3::SetInt( int32_t value )
{ SetValue<int32_t>( value, KV3_TYPEEX_INT, KV3_SUBTYPE_INT32 ); }

void KeyValues3::SetUInt( uint32_t value )
{ SetValue<uint32_t>( value, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT32 ); }

void KeyValues3::SetInt64( int64_t value )
{ SetValue<int64_t>( value, KV3_TYPEEX_INT, KV3_SUBTYPE_INT64 ); }

void KeyValues3::SetUInt64( uint64_t value )
{ SetValue<uint64_t>( value, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT64 ); }

void KeyValues3::SetFloat( float value )
{ SetValue<float>( value, KV3_TYPEEX_DOUBLE, KV3_SUBTYPE_FLOAT32 ); }

void KeyValues3::SetDouble( double value )
{ SetValue<double>( value, KV3_TYPEEX_DOUBLE, KV3_SUBTYPE_FLOAT64 ); }

void* KeyValues3::GetPointer( void *defaultValue ) const
{ return ( GetSubType() == KV3_SUBTYPE_POINTER ) ? ( void* )m_Data.m_UInt : defaultValue; }

void KeyValues3::SetPointer( void* ptr )
{ SetValue<uint64_t>( ( uint64_t )ptr, KV3_TYPEEX_UINT, KV3_SUBTYPE_POINTER ); }

CUtlStringToken KeyValues3::GetStringToken( CUtlStringToken defaultValue ) const
{ return ( GetSubType() == KV3_SUBTYPE_STRING_TOKEN ) ? CUtlStringToken( ( uint32 )m_Data.m_UInt ) : defaultValue; }

void KeyValues3::SetStringToken( CUtlStringToken token )
{ SetValue<uint32>( token.GetHashCode(), KV3_TYPEEX_UINT, KV3_SUBTYPE_STRING_TOKEN ); }

CEntityHandle KeyValues3::GetEHandle( CEntityHandle defaultValue ) const
{ return ( GetSubType() == KV3_SUBTYPE_EHANDLE ) ? CEntityHandle( ( uint32 )m_Data.m_UInt ) : defaultValue; }

void KeyValues3::SetEHandle( CEntityHandle ehandle )
{ SetValue<uint32>( ehandle.ToInt(), KV3_TYPEEX_UINT, KV3_SUBTYPE_EHANDLE ); }

Vector KeyValues3::GetVector( const Vector &defaultValue ) const
{ return GetVecBasedObj<Vector>( 3, defaultValue ); }

Vector2D KeyValues3::GetVector2D( const Vector2D &defaultValue ) const
{ return GetVecBasedObj<Vector2D>( 2, defaultValue ); }

Vector4D KeyValues3::GetVector4D( const Vector4D &defaultValue ) const
{ return GetVecBasedObj<Vector4D>( 4, defaultValue ); }

Quaternion KeyValues3::GetQuaternion( const Quaternion &defaultValue ) const
{ return GetVecBasedObj<Quaternion>( 4, defaultValue ); }

QAngle KeyValues3::GetQAngle( const QAngle &defaultValue ) const
{ return GetVecBasedObj<QAngle>( 3, defaultValue ); }

matrix3x4_t KeyValues3::GetMatrix3x4( const matrix3x4_t &defaultValue ) const
{ return GetVecBasedObj<matrix3x4_t>( 3*4, defaultValue ); }

void KeyValues3::SetVector( const Vector &vec )
{ SetVecBasedObj<Vector>( vec, 3, KV3_SUBTYPE_VECTOR ); }

void KeyValues3::SetVector2D( const Vector2D &vec2d )
{ SetVecBasedObj<Vector2D>( vec2d, 2, KV3_SUBTYPE_VECTOR2D ); }

void KeyValues3::SetVector4D( const Vector4D &vec4d )
{ SetVecBasedObj<Vector4D>( vec4d, 4, KV3_SUBTYPE_VECTOR4D ); }

void KeyValues3::SetQuaternion( const Quaternion &quat )
{ SetVecBasedObj<Quaternion>( quat, 4, KV3_SUBTYPE_QUATERNION ); }

void KeyValues3::SetQAngle( const QAngle &ang )
{ SetVecBasedObj<QAngle>( ang, 3, KV3_SUBTYPE_QANGLE ); }

void KeyValues3::SetMatrix3x4( const matrix3x4_t &matrix )
{ SetVecBasedObj<matrix3x4_t>( matrix, 3*4, KV3_SUBTYPE_MATRIX3X4 ); }

void KeyValues3::SetToEmptyArray()
{ PrepareForType( KV3_TYPEEX_ARRAY, KV3_SUBTYPE_ARRAY ); }

KeyValues3* KeyValues3::ArrayInsertElementAfter( int elem )
{ return ArrayInsertElementBefore( elem + 1 ); }

void KeyValues3::ArrayRemoveElement( int elem )
{ ArrayRemoveElements( elem, 1 ); }

bool KeyValues3::GetMemberBool( const CKV3MemberName &name, bool defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetBool( defaultValue ) : defaultValue; }

int8_t KeyValues3::GetMemberChar( const CKV3MemberName &name, int8_t defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetChar( defaultValue ) : defaultValue; }

uint32_t KeyValues3::GetMemberUChar32( const CKV3MemberName &name, uint32_t defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetUChar32( defaultValue ) : defaultValue; }

int8 KeyValues3::GetMemberInt8( const CKV3MemberName &name, int8 defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetInt8( defaultValue ) : defaultValue; }

uint8 KeyValues3::GetMemberUInt8( const CKV3MemberName &name, uint8 defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetUInt8( defaultValue ) : defaultValue; }

int16 KeyValues3::GetMemberShort( const CKV3MemberName &name, int16 defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetShort( defaultValue ) : defaultValue; }

uint16 KeyValues3::GetMemberUShort( const CKV3MemberName &name, uint16 defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetUShort( defaultValue ) : defaultValue; }

int32 KeyValues3::GetMemberInt( const CKV3MemberName &name, int32 defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetInt( defaultValue ) : defaultValue; }

uint32 KeyValues3::GetMemberUInt( const CKV3MemberName &name, uint32 defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetUInt( defaultValue ) : defaultValue; }

int64 KeyValues3::GetMemberInt64( const CKV3MemberName &name, int64 defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetInt64( defaultValue ) : defaultValue; }

uint64 KeyValues3::GetMemberUInt64( const CKV3MemberName &name, uint64 defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetUInt64( defaultValue ) : defaultValue; }

float32 KeyValues3::GetMemberFloat( const CKV3MemberName &name, float32 defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetFloat( defaultValue ) : defaultValue; }

float64 KeyValues3::GetMemberDouble( const CKV3MemberName &name, float64 defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetDouble( defaultValue ) : defaultValue; }

void * KeyValues3::GetMemberPointer( const CKV3MemberName &name, void *defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetPointer( defaultValue ) : defaultValue; }

CUtlStringToken KeyValues3::GetMemberStringToken( const CKV3MemberName &name, CUtlStringToken defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetStringToken( defaultValue ) : defaultValue; }

CEntityHandle KeyValues3::GetMemberEHandle( const CKV3MemberName &name, CEntityHandle defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetEHandle( defaultValue ) : defaultValue; }

const char * KeyValues3::GetMemberString( const CKV3MemberName &name, const char *defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetString( defaultValue ) : defaultValue; }

Color KeyValues3::GetMemberColor( const CKV3MemberName &name, const Color &defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetColor( defaultValue ) : defaultValue; }

Vector KeyValues3::GetMemberVector( const CKV3MemberName &name, const Vector &defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetVector( defaultValue ) : defaultValue; }

Vector2D KeyValues3::GetMemberVector2D( const CKV3MemberName &name, const Vector2D &defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetVector2D( defaultValue ) : defaultValue; }

Vector4D KeyValues3::GetMemberVector4D( const CKV3MemberName &name, const Vector4D &defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetVector4D( defaultValue ) : defaultValue; }

Quaternion KeyValues3::GetMemberQuaternion( const CKV3MemberName &name, const Quaternion &defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetQuaternion( defaultValue ) : defaultValue; }

QAngle KeyValues3::GetMemberQAngle( const CKV3MemberName &name, const QAngle &defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetQAngle( defaultValue ) : defaultValue; }

matrix3x4_t KeyValues3::GetMemberMatrix3x4( const CKV3MemberName &name, const matrix3x4_t &defaultValue ) const
{ auto kv = FindMember( name ); return kv ? kv->GetMatrix3x4( defaultValue ) : defaultValue; }

void KeyValues3::SetMemberToNull( const CKV3MemberName &name )
{ FindOrCreateMember( name )->SetToNull(); }

void KeyValues3::SetMemberToEmptyArray( const CKV3MemberName &name )
{ FindOrCreateMember( name )->SetToEmptyArray(); }

void KeyValues3::SetMemberToEmptyTable( const CKV3MemberName &name )
{ FindOrCreateMember( name )->SetToEmptyTable(); }

void KeyValues3::SetMemberToBinaryBlob( const CKV3MemberName &name, const byte *blob, int size )
{ FindOrCreateMember( name )->SetToBinaryBlob( blob, size ); }

void KeyValues3::SetMemberToBinaryBlobExternal( const CKV3MemberName &name, const byte *blob, int size, bool free_mem )
{ FindOrCreateMember( name )->SetToBinaryBlobExternal( blob, size, free_mem ); }

void KeyValues3::SetMemberToCopyOfValue( const CKV3MemberName &name, KeyValues3 *other )
{ FindOrCreateMember( name )->CopyFrom( other ); }

void KeyValues3::SetMemberBool( const CKV3MemberName &name, bool value )
{ FindOrCreateMember( name )->SetBool( value ); }

void KeyValues3::SetMemberChar( const CKV3MemberName &name, int8_t value )
{ FindOrCreateMember( name )->SetChar( value ); }

void KeyValues3::SetMemberUChar32( const CKV3MemberName &name, uint32_t value )
{ FindOrCreateMember( name )->SetUChar32( value ); }

void KeyValues3::SetMemberInt8( const CKV3MemberName &name, int8 value )
{ FindOrCreateMember( name )->SetInt8( value ); }

void KeyValues3::SetMemberUInt8( const CKV3MemberName &name, uint8 value )
{ FindOrCreateMember( name )->SetUInt8( value ); }

void KeyValues3::SetMemberShort( const CKV3MemberName &name, int16 value )
{ FindOrCreateMember( name )->SetShort( value ); }

void KeyValues3::SetMemberUShort( const CKV3MemberName &name, uint16 value )
{ FindOrCreateMember( name )->SetUShort( value ); }

void KeyValues3::SetMemberInt( const CKV3MemberName &name, int32 value )
{ FindOrCreateMember( name )->SetInt( value ); }

void KeyValues3::SetMemberUInt( const CKV3MemberName &name, uint32 value )
{ FindOrCreateMember( name )->SetUInt( value ); }

void KeyValues3::SetMemberInt64( const CKV3MemberName &name, int64 value )
{ FindOrCreateMember( name )->SetInt64( value ); }

void KeyValues3::SetMemberUInt64( const CKV3MemberName &name, uint64 value )
{ FindOrCreateMember( name )->SetUInt64( value ); }

void KeyValues3::SetMemberFloat( const CKV3MemberName &name, float32 value )
{ FindOrCreateMember( name )->SetFloat( value ); }

void KeyValues3::SetMemberDouble( const CKV3MemberName &name, float64 value )
{ FindOrCreateMember( name )->SetDouble( value ); }

void KeyValues3::SetMemberPointer( const CKV3MemberName &name, void *ptr )
{ FindOrCreateMember( name )->SetPointer( ptr ); }

void KeyValues3::SetMemberStringToken( const CKV3MemberName &name, CUtlStringToken token )
{ FindOrCreateMember( name )->SetStringToken( token ); }

void KeyValues3::SetMemberEHandle( const CKV3MemberName &name, CEntityHandle ehandle )
{ FindOrCreateMember( name )->SetEHandle( ehandle ); }

void KeyValues3::SetMemberString( const CKV3MemberName &name, const char *pString, KV3SubType_t subtype )
{ FindOrCreateMember( name )->SetString( pString, subtype ); }

void KeyValues3::SetMemberStringExternal( const CKV3MemberName &name, const char *pString, KV3SubType_t subtype )
{ FindOrCreateMember( name )->SetStringExternal( pString, subtype ); }

void KeyValues3::SetMemberColor( const CKV3MemberName &name, const Color &color )
{ FindOrCreateMember( name )->SetColor( color ); }

void KeyValues3::SetMemberVector( const CKV3MemberName &name, const Vector &vec )
{ FindOrCreateMember( name )->SetVector( vec ); }

void KeyValues3::SetMemberVector2D( const CKV3MemberName &name, const Vector2D &vec2d )
{ FindOrCreateMember( name )->SetVector2D( vec2d ); }

void KeyValues3::SetMemberVector4D( const CKV3MemberName &name, const  Vector4D &vec4d )
{ FindOrCreateMember( name )->SetVector4D( vec4d ); }

void KeyValues3::SetMemberQuaternion( const CKV3MemberName &name, const Quaternion &quat )
{ FindOrCreateMember( name )->SetQuaternion( quat ); }

void KeyValues3::SetMemberQAngle( const CKV3MemberName &name, const QAngle &ang )
{ FindOrCreateMember( name )->SetQAngle( ang ); }

void KeyValues3::SetMemberMatrix3x4( const CKV3MemberName &name, const matrix3x4_t &matrix )
{ FindOrCreateMember( name )->SetMatrix3x4( matrix ); }

KeyValues3::Data_t::Data_t()
: m_nMemory(0)
{
}

int KeyValues3::GetClusterElement() const
{ return m_nClusterElement; }

void KeyValues3::SetClusterElement( int element )
{ m_bContextIndependent = (element == -1); m_nClusterElement = element; }

CKeyValues3Iterator::CKeyValues3Iterator()
: m_Stack()
{}

CKeyValues3Iterator::CKeyValues3Iterator( KeyValues3 *kv )
: CKeyValues3Iterator()
{ Init( kv ); }

KeyValues3 * CKeyValues3Iterator::Get() const
{ return IsValid() ? m_Stack[m_Stack.Count() - 1].m_pKV : nullptr; }

bool CKeyValues3Iterator::IsValid() const
{ return m_Stack.Count() > 0; }

CKeyValues3Array::~CKeyValues3Array()
{ Free(); }

int CKeyValues3Array::GetClusterElement() const
{ return m_nClusterElement; }

void CKeyValues3Array::SetClusterElement( int element )
{ m_nClusterElement = element; }

CKeyValues3Array::Element_t * CKeyValues3Array::Base()
{ return IsBaseStatic() ? &m_StaticElements[0] : m_pDynamicElements; }

CKeyValues3Array::Element_t const * CKeyValues3Array::Base() const
{ return const_cast<CKeyValues3Array *>(this)->Base(); }

int CKeyValues3Array::Count() const
{ return m_nCount; }

void CKeyValues3Array::Free( bool clearing_context )
{ PurgeBuffers(); }

size_t CKeyValues3Array::GetAllocatedSize() const
{ return TotalSizeOf( m_nInitialSize ); }

int CKeyValues3Array::GetAllocatedChunks() const
{ return m_nAllocatedChunks; }

bool CKeyValues3Array::IsBaseStatic()
{ return !m_bIsDynamicallySized; }

size_t CKeyValues3Array::GetAllocatedBytesSize() const
{ return TotalSizeOfData( GetAllocatedChunks() ); }

CKeyValues3Table::~CKeyValues3Table()
{ Free(); }

int CKeyValues3Table::GetClusterElement() const
{ return m_nClusterElement; }

void CKeyValues3Table::SetClusterElement( int element )
{ m_nClusterElement = element; }

int CKeyValues3Table::GetMemberCount() const
{ return m_nCount; }

bool CKeyValues3Table::HasInvalidMemberNames() const
{ return m_bHasInvalidMemberNames; }

void CKeyValues3Table::SetHasInvalidMemberNames( bool state )
{ m_bHasInvalidMemberNames = state; }

void CKeyValues3Table::Free( bool clearing_context )
{ PurgeBuffers(); }

size_t CKeyValues3Table::GetAllocatedSize() const
{ return TotalSizeOf( m_nInitialSize ); }

int CKeyValues3Table::GetAllocatedChunks() const
{ return m_nAllocatedChunks; }

bool CKeyValues3Table::IsBaseStatic()
{ return !m_bIsDynamicallySized; }

size_t CKeyValues3Table::GetAllocatedBytesSize() const
{ return TotalSizeOfData( GetAllocatedChunks() ); }

void * CKeyValues3Table::Base()
{ return IsBaseStatic() ? &m_StaticBuffer : m_pDynamicBuffer; }

CKeyValues3Table::Hash_t * CKeyValues3Table::HashesBase()
{ return reinterpret_cast<Hash_t *>((uint8 *)Base() + OffsetToHashesBase( GetAllocatedChunks() )); }

CKeyValues3Table::Member_t * CKeyValues3Table::MembersBase()
{ return reinterpret_cast<Member_t *>((uint8 *)Base() + OffsetToMembersBase( GetAllocatedChunks() )); }

CKeyValues3Table::Name_t * CKeyValues3Table::NamesBase()
{ return reinterpret_cast<Name_t *>((uint8 *)Base() + OffsetToNamesBase( GetAllocatedChunks() )); }

CKeyValues3Table::Flags_t * CKeyValues3Table::FlagsBase()
{ return reinterpret_cast<Flags_t *>((uint8 *)Base() + OffsetToFlagsBase( GetAllocatedChunks() )); }

const void * CKeyValues3Table::Base() const
{ return const_cast<CKeyValues3Table *>(this)->Base(); }

const CKeyValues3Table::Hash_t * CKeyValues3Table::HashesBase() const
{ return const_cast<CKeyValues3Table *>(this)->HashesBase(); }

const CKeyValues3Table::Member_t * CKeyValues3Table::MembersBase() const
{ return const_cast<CKeyValues3Table *>(this)->MembersBase(); }

const CKeyValues3Table::Name_t * CKeyValues3Table::NamesBase() const
{ return const_cast<CKeyValues3Table *>(this)->NamesBase(); }

const CKeyValues3Table::Flags_t * CKeyValues3Table::FlagsBase() const
{ return const_cast<CKeyValues3Table *>(this)->FlagsBase(); }

CKeyValues3Table::kv3tablefastsearch_t::kv3tablefastsearch_t()
: m_ignore( false ), m_ignores_counter( 0 )
{}

CKeyValues3Table::kv3tablefastsearch_t::~kv3tablefastsearch_t()
{ Clear(); }

void CKeyValues3Table::kv3tablefastsearch_t::Clear()
{
	m_ignore = false;
	m_ignores_counter = 0;
	m_member_ids.RemoveAll();
}

unsigned int CKeyValues3Table::kv3tablefastsearch_t::EmptyHashFunctor::operator()( uint32 n ) const
{ return n; }

CKV3ArenaBase::~CKV3ArenaBase()
{ Purge(); }

CKV3Arena::~CKV3Arena()
{ Purge(); }

bool CKV3Arena::IsMetaDataEnabled() const
{ return m_bMetaDataEnabled; }

bool CKV3Arena::IsFormatConverted() const
{ return m_bFormatConverted; }

bool CKV3Arena::IsRootAvailabe() const
{ return m_bRootAvailabe; }

CUtlBuffer& CKV3Arena::GetBinaryData()
{ return m_BinaryData; }

IErrorListener* CKV3Arena::GetParsingErrorListener() const
{ return m_pParsingErrorListener; }

void CKV3Arena::SetParsingErrorListener( IErrorListener* listener )
{ m_pParsingErrorListener = listener; }

bool CKV3Arena::IsArrayRawAllocated( CKeyValues3Array *element )
{ return m_RawArrayEntries.IsWithinRange( element ); }

bool CKV3Arena::IsTableRawAllocated( CKeyValues3Table *element )
{ return m_RawTableEntries.IsWithinRange( element ); }

CKeyValues3Array * CKV3Arena::AllocArray( int initial_size )
{ return RawAlloc( m_RawArrayEntries, m_PartialArrayClusters, m_FullArrayClusters, initial_size ); }

CKeyValues3Table * CKV3Arena::AllocTable( int initial_size )
{ return RawAlloc( m_RawTableEntries, m_PartialTableClusters, m_FullTableClusters, initial_size ); }

void CKV3Arena::FreeArray( CKeyValues3Array *element )
{ Free( element, m_PartialArrayClusters, m_FullArrayClusters ); }

void CKV3Arena::FreeTable( CKeyValues3Table *element )
{ Free( element, m_PartialTableClusters, m_FullTableClusters ); }
