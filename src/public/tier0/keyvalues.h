#ifndef KEYVALUES_H
#define KEYVALUES_H

#ifdef _WIN32
#pragma once
#endif

#include "color.h"
#include "exprevaluator.h"
#include "keyvaluessystem.h"
#include "platform.h"
#include "public/filesystem/filesystem.h"
#include "utlbuffer.h"

class KeyValues;
class IKeyValuesErrorSpew;
class IKeyValuesDumpContext;

#define KV_BINARY_POOLED_FORMAT 0xAA

#define FOR_EACH_SUBKEY( kvRoot, kvSubKey ) \
	for ( KeyValues * kvSubKey = kvRoot->GetFirstSubKey(); kvSubKey != NULL; kvSubKey = kvSubKey->GetNextKey() )

#define FOR_EACH_TRUE_SUBKEY( kvRoot, kvSubKey ) \
	for ( KeyValues * kvSubKey = kvRoot->GetFirstTrueSubKey(); kvSubKey != NULL; kvSubKey = kvSubKey->GetNextTrueSubKey() )

#define FOR_EACH_VALUE( kvRoot, kvValue ) \
	for ( KeyValues * kvValue = kvRoot->GetFirstValue(); kvValue != NULL; kvValue = kvValue->GetNextValue() )

PLATFORM_INTERFACE KeyValues *KeyValuesFromJSON( CUtlBuffer *buf, bool allow_unterminated = false, bool *parsed_successfully = nullptr );
PLATFORM_INTERFACE bool KeyValuesToJSON( KeyValues *kv, CUtlBuffer *buf );

class CTemporaryKeyValues
{
public:
	CTemporaryKeyValues();
	~CTemporaryKeyValues();

private:
	KeyValues *m_pKeyValues;
	HTemporaryKeyValueAllocationScope m_hScope;
};

#pragma pack(push, 1)
class CKeyValues_Data
{
public:
	// Data type
	enum types_t
	{
		TYPE_NONE = 0,
		TYPE_STRING,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_PTR,
		TYPE_WSTRING,
		TYPE_COLOR,
		TYPE_UINT64,
		TYPE_COMPILED_INT_BYTE,			// hack to collapse 1 byte ints in the compiled format
		TYPE_COMPILED_INT_0,			// hack to collapse 0 in the compiled format
		TYPE_COMPILED_INT_1,			// hack to collapse 1 in the compiled format
		TYPE_NUMTYPES,
	};

protected:
	// AMNOTE: Valve scrapped the public-facing version of these,
	// so we have to expose as protected for the scaffolds below in KeyValues.
	char const *Internal_GetString( const char *defaultValue, char *szBuf, size_t maxlen );
	const wchar_t *Internal_GetWString( const wchar_t *defaultValue, wchar_t *szBuf, size_t maxlen );

protected:
	IKeyValuesSystem *KVSystem() const;

	Color ResolveColorValue() const;
	float ResolveFloatValue() const;
	int ResolveIntValue() const;
	uint64_t ResolveUint64Value() const;
	const char *ResolveStringValue() const;
	const wchar_t *ResolveWStringValue() const;
	KeyValues *ResolveSubKeyValue() const;
	void *ResolvePtrValue() const;

	void Internal_ClearAll();
	void Internal_ClearData();

	void Internal_CopyData( const CKeyValues_Data &kv );
	void Internal_CopyName( const CKeyValues_Data &kv );

	bool Internal_IsEqual( CKeyValues_Data *pOther );

	const char *Internal_GetName() const;

	KeyValues *Internal_GetSubKey() const;
	void Internal_SetSubKey( KeyValues *subkey );

	HKeySymbol Internal_GetNameSymbol() const;
	HKeySymbol Internal_GetNameSymbolCaseSensitive() const;

	bool Internal_SetName( char const *szName );
	void Internal_SetNameFrom( CKeyValues_Data const &pOther );

	Color Internal_GetColor( Color defaultClr ) const;
	float Internal_GetFloat() const;
	int Internal_GetInt() const;
	uint64_t Internal_GetUint64() const;
	void *Internal_GetPtr() const;
	
	void Internal_SetColor( Color value );
	void Internal_SetFloat( float value );
	void Internal_SetInt( int value );
	void Internal_SetUint64( uint64_t value );
	void Internal_SetString( const char *value );
	void Internal_SetWString( const wchar_t *value );
	void Internal_SetPtr( void *value );

	void Internal_SetFromString( types_t type, const char *value );

	bool Internal_HasEscapeSequences() const;
	void Internal_SetHasEscapeSequences( bool state );

	union
	{
		int m_iValue;
		float m_flValue;
		void *m_pValue;
		unsigned char m_Color[4];
		uintptr_t m_uValue;
		const char *m_sValue;
		const wchar_t *m_wsValue;
		KeyValues *m_pSub;
	};

	IKeyValuesSystem *m_pKVSystem;

	uint32_t m_iKeyNameCaseSensitive : 24;
	uint32_t m_iDataType : 3;
	uint32_t m_bHasEscapeSequences : 1;
	uint32_t m_bAllocatedExternalMemory : 1;
	uint32_t m_bKeySymbolCaseSensitiveMatchesCaseInsensitive : 1;
	uint32_t m_bStoredSubKey : 1;
	uint32_t m_bOwnsKeyValuesSystem : 1;
};

class KeyValues : public CKeyValues_Data
{
public:
	//
	// AutoDelete class to automatically free the keyvalues.
	// Simply construct it with the keyvalues you allocated and it will free them when falls out of scope.
	// When you decide that keyvalues shouldn't be deleted call Assign(NULL) on it.
	// If you constructed AutoDelete(NULL) you can later assign the keyvalues to be deleted with Assign(pKeyValues).
	//
	class AutoDelete
	{
	public:
		explicit AutoDelete( KeyValues *pKeyValues );
		explicit AutoDelete( const char *pchKVName );
		~AutoDelete( void );
		void Assign( KeyValues *pKeyValues );
		KeyValues *operator->();
		operator KeyValues *();
	private:
		AutoDelete( AutoDelete const &x ); // forbid
		AutoDelete &operator= ( AutoDelete const &x ); // forbid
	protected:
		KeyValues *m_pKeyValues;
	};

	//
	// AutoDeleteInline is useful when you want to hold your keyvalues object inside
	// and delete it right after using.
	// You can also pass temporary KeyValues object as an argument to a function by wrapping it into KeyValues::AutoDeleteInline
	// instance:   call_my_function( KeyValues::AutoDeleteInline( new KeyValues( "test" ) ) )
	//
	class AutoDeleteInline : public AutoDelete
	{
	public:
		explicit AutoDeleteInline( KeyValues *pKeyValues );
		operator KeyValues *() const;
		KeyValues *Get() const;
	};

	// AMNOTE: Value names unknown, the constructor taking it doesn't read it
	enum KV_DISPOSABLE_STRING_TABLE_STATE {};

	// Quick setup constructors
	// The destructor deletes kvsystem when bOwnsKeyValuesSystem is set; child keys don't own it
	KeyValues( const char *setName, IKeyValuesSystem *kvsystem = NULL, bool bOwnsKeyValuesSystem = false );
	// AMNOTE: Gives the KeyValues a string table of its own instead of the global one
	KeyValues( const char *setName, KV_DISPOSABLE_STRING_TABLE_STATE );
	// AMNOTE: Copies only the name of pOther, not its system, keys or values, and doesn't read the bool
	KeyValues( const KeyValues *pOther, bool );
	KeyValues( const char *setName, const char *firstKey, const char *firstValue );
	KeyValues( const char *setName, const char *firstKey, const wchar_t *firstValue );
	KeyValues( const char *setName, const char *firstKey, int firstValue );
	KeyValues( const char *setName, const char *firstKey, bool firstValue );
	KeyValues( const char *setName, const char *firstKey, float firstValue );
	KeyValues( const char *setName, const char *firstKey, uint64_t firstValue );
	KeyValues( const char *setName, const char *firstKey, const char *firstValue, const char *secondKey, const char *secondValue );

	~KeyValues();

	// Section name
	const char *GetName() const;
	void SetName( const char *setName );

	// gets the name as a unique int
	HKeySymbol GetNameSymbol() const;
	HKeySymbol GetNameSymbolCaseSensitive() const;

	// The system that owns the key's name symbols
	IKeyValuesSystem *GetKeyValuesSystem() const;

	// File access. Set UsesEscapeSequences true, if resource file/buffer uses Escape Sequences (eg \n, \t)
	void UsesEscapeSequences( bool state ); // default false
	bool LoadFromFile( IFileSystem *filesystem, const char *resourceName, const char *pathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, void *pSymbolProcContext = NULL, const char *pszNameForEmptyKeys = NULL );
	bool SaveToFile( IFileSystem *filesystem, const char *resourceName, const char *pathID = NULL, bool bAllowEmptyString = false, bool binaryWrite = true );
	// Read from a buffer...  Note that the buffer must be null terminated
	bool LoadFromBuffer( char const *resourceName, const char *pBuffer, IFileSystem *pFileSystem = NULL, const char *pPathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, IKeyValuesErrorSpew *pErrorSpew = NULL, void *pSymbolProcContext = NULL, const char *pszNameForEmptyKeys = NULL );

	// Read from a utlbuffer...
	bool LoadFromBuffer( char const *resourceName, CUtlBuffer &buf, IFileSystem *pFileSystem = NULL, const char *pPathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, IKeyValuesErrorSpew *pErrorSpew = NULL, void *pSymbolProcContext = NULL, const char *pszNameForEmptyKeys = NULL );

	static CTemporaryKeyValues *LoadTemporaryFromBuffer( bool bUsesEscapeSequences, char const *resourceName, const char *pBuffer, IFileSystem *pFileSystem = NULL, const char *pPathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, IKeyValuesErrorSpew *pErrorSpew = NULL, void *pSymbolProcContext = NULL, const char *pszNameForEmptyKeys = NULL );
	static CTemporaryKeyValues *LoadTemporaryFromBuffer( bool bUsesEscapeSequences, char const *resourceName, CUtlBuffer &buf, IFileSystem *pFileSystem = NULL, const char *pPathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, void *pSymbolProcContext = NULL, const char *pszNameForEmptyKeys = NULL );

	static CTemporaryKeyValues *LoadTemporaryFromFile( bool bUsesEscapeSequences, IFileSystem *filesystem, const char *resourceName, const char *pathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, void *pSymbolProcContext = NULL, const char *pszNameForEmptyKeys = NULL );

	// Find a keyValue, create it if it is not found.
	// Set bCreate to true to create the key if it doesn't already exist (which ensures a valid pointer will be returned)
	KeyValues *FindKey( const char *keyName, bool bCreate = false );
	const KeyValues *FindKey( const char *keyName ) const;
	KeyValues *FindKey( HKeySymbol keySymbol );
	KeyValues *FindKeyAndParent( const char *keyName, KeyValues **pParent, bool );
	bool FindAndDeleteSubKey( const char *keyName );

	KeyValues *CreateNewKey();		// creates a new key, with an autogenerated name.  name is guaranteed to be an integer, of value 1 higher than the highest other integer key name
	KeyValues *CreatePeerKey( const char *keyName );

	KeyValues *AddKey( const char *keyName );
	void AddSubKey( KeyValues *pSubkey );	// Adds a subkey. Make sure the subkey isn't a child of some other keyvalues
	void AddSubkeyUsingKnownLastChild( KeyValues *pSubkey, KeyValues *pChild );

	KeyValues *CreateKeyUsingKnownLastChild( const char *keyName, KeyValues *pChild );

	void RemoveSubKey( KeyValues *subKey, bool, bool );	// removes a subkey from the list, DOES NOT DELETE IT
	void RemoveOptionalSubKey( KeyValues *subKey );
	void InsertSubKey( int nIndex, KeyValues *pSubKey ); // Inserts the given sub-key before the Nth child location
	bool ContainsSubKey( KeyValues *pSubKey ); // Returns true if this key values contains the specified sub key, false otherwise.
	void SwapSubKey( KeyValues *pExistingSubKey, KeyValues *pNewSubKey );	// Swaps an existing subkey for a new one, DOES NOT DELETE THE OLD ONE but takes ownership of the new one
	void ElideSubKey( KeyValues *pSubKey );	// Removes a subkey but inserts all of its children in its place, in-order (flattens a tree, like firing a manager!)

	// Key iteration.
	//
	// NOTE: GetFirstSubKey/GetNextKey will iterate keys AND values. Use the functions 
	// below if you want to iterate over just the keys or just the values.
	//
	KeyValues *GetFirstSubKey();	// returns the first subkey in the list
	KeyValues *FindLastSubKey();
	KeyValues *GetNextKey();		// returns the next subkey
	void SetNextKey( KeyValues *pDat );

	//
	// These functions can be used to treat it like a true key/values tree instead of 
	// confusing values with keys.
	//
	// So if you wanted to iterate all subkeys, then all values, it would look like this:
	//     for ( KeyValues *pKey = pRoot->GetFirstTrueSubKey(); pKey; pKey = pKey->GetNextTrueSubKey() )
	//     {
	//		   Msg( "Key name: %s\n", pKey->GetName() );
	//     }
	//     for ( KeyValues *pValue = pRoot->GetFirstValue(); pKey; pKey = pKey->GetNextValue() )
	//     {
	//         Msg( "Int value: %d\n", pValue->GetInt() );  // Assuming pValue->GetDataType() == TYPE_INT...
	//     }
	KeyValues *GetFirstTrueSubKey();
	KeyValues *GetNextTrueSubKey();

	KeyValues *GetFirstValue();	// When you get a value back, you can use GetX and pass in NULL to get the value.
	KeyValues *GetNextValue();


	// Data access
	int GetInt( const char *keyName = NULL, int defaultValue = 0 ) const;
	uint64_t GetUint64( const char *keyName = NULL, uint64_t defaultValue = 0 ) const;
	float GetFloat( const char *keyName = NULL, float defaultValue = 0.0f ) const;
	const char *GetString( const char *keyName, const char *defaultValue = "", char *pszOut = NULL, size_t maxlen = 0 );
	const wchar_t *GetWString( const char *keyName = NULL, const wchar_t *defaultValue = L"", wchar_t *pszOut = NULL, size_t maxlen = 0 );
	void *GetPtr( const char *keyName = NULL, void *defaultValue = (void *)0 ) const;
	Color GetColor( const char *keyName = NULL, const Color &defaultColor = Color( 0, 0, 0, 0 ) ) const;
	bool GetBool( const char *keyName = NULL, bool defaultValue = false ) const;
	bool IsEmpty( const char *keyName = NULL ) const;

	// Data access
	int GetInt( HKeySymbol keySymbol, int defaultValue = 0 );
	uint64_t GetUint64( HKeySymbol keySymbol, uint64_t defaultValue = 0 );
	float GetFloat( HKeySymbol keySymbol, float defaultValue = 0.0f );
	const char *GetString( HKeySymbol keySymbol, const char *defaultValue = "", char *pszOut = NULL, size_t maxlen = 0 );
	const wchar_t *GetWString( HKeySymbol keySymbol, const wchar_t *defaultValue = L"", wchar_t *pszOut = NULL, size_t maxlen = 0 );
	void *GetPtr( HKeySymbol keySymbol, void *defaultValue = (void *)0 );
	Color GetColor( HKeySymbol keySymbol, const Color &defaultColor = Color( 0, 0, 0, 0 ) );
	bool GetBool( HKeySymbol keySymbol, bool defaultValue = false );
	bool IsEmpty( HKeySymbol keySymbol );

	// Key writing
	void SetWString( const char *keyName, const wchar_t *value );
	void SetString( const char *keyName, const char *value );
	void SetInt( const char *keyName, int value );
	void SetUint64( const char *keyName, uint64_t value );
	void SetFloat( const char *keyName, float value );
	void SetPtr( const char *keyName, void *value );
	void SetColor( const char *keyName, Color value );
	void SetBool( const char *keyName, bool value );

	// Adds key value pair
	void AddString( const char *keyName, const char *value );
	void AddInt( const char *keyName, int value );
	void AddUint64( const char *keyName, uint64_t value );
	void AddFloat( const char *keyName, float value );
	void AddPtr( const char *keyName, void *value );

	// Memory allocation (optimized)
	void *operator new(size_t iAllocSize);
	void *operator new(size_t iAllocSize, int nBlockUse, const char *pFileName, int nLine);
	void operator delete(void *pMem);
	void operator delete(void *pMem, int nBlockUse, const char *pFileName, int nLine);

	KeyValues &operator=( const KeyValues &src );

	void RecursiveSaveToFile( CUtlBuffer &buf, int indentLevel, bool bSortKeys = false, bool bAllowEmptyString = false ) const;
	void RecursiveSaveToLocalizationFile( IFileSystem *filesystem, void *buf, int indentLevel, bool bAllowEmptyString = false );

	bool WriteAsBinary( CUtlBuffer &buffer ) const;
	bool WriteAsBinaryFiltered( CUtlBuffer &buffer );
	bool ReadAsBinary( CUtlBuffer &buffer );
	bool ReadAsBinaryFiltered( CUtlBuffer &buffer );
	static CTemporaryKeyValues *ReadTemporaryAsBinary( bool bUsesEscapeSequences, CUtlBuffer &buffer );

	// Allocate & create a new copy of the keys
	KeyValues *MakeCopy( void ) const;

	// Make a new copy of all subkeys, add them all to the passed-in keyvalues
	void CopySubkeys( KeyValues *pParent ) const;

	// Clear out all subkeys, and the current value
	void Clear( void );

	bool IsEqual( KeyValues *pOther );

	int Count( void ) const;

	KeyValues *Element( int nIndex );

	CKeyValues_Data::types_t GetDataType( const char *keyName = NULL ) const;

	// unpack a key values list into a structure
	void UnpackIntoStructure( struct KeyValuesUnpackStructure const *pUnpackTable, void *pDest );

	// Process conditional keys for widescreen support.
	bool ProcessResolutionKeys( const char *pResString );

	// Dump keyvalues recursively into a dump context
	bool Dump( IKeyValuesDumpContext *pDump, int nIndentLevel = 0 );

	// Merge operations describing how two keyvalues can be combined
	enum MergeKeyValuesOp_t
	{
		MERGE_KV_ALL,
		MERGE_KV_UPDATE,	// update values are copied into storage, adding new keys to storage or updating existing ones
		MERGE_KV_DELETE,	// update values specify keys that get deleted from storage
		MERGE_KV_BORROW,	// update values only update existing keys in storage, keys in update that do not exist in storage are discarded
		MERGE_KV_ADD_ONLY
	};

	void MergeFrom( const KeyValues *kvMerge, MergeKeyValuesOp_t eOp = MERGE_KV_ALL );
	CTemporaryKeyValues *MergeFromTemporary( const KeyValues *kvMerge, MergeKeyValuesOp_t eOp = MERGE_KV_ALL ) const;

	// Assign keyvalues from a string
	static KeyValues *FromString( char const *szName, char const *szStringVal, char const **ppEndOfParse = NULL );


private:
	KeyValues( KeyValues & ); // prevent copy constructor being used

	KeyValues *m_pPeer;	// pointer to next key in list
};
#pragma pack(pop)

typedef KeyValues::AutoDelete KeyValuesAD;

enum KeyValuesUnpackDestinationTypes_t
{
	UNPACK_TYPE_FLOAT,										// dest is a float
	UNPACK_TYPE_VECTOR,										// dest is a Vector
	UNPACK_TYPE_VECTOR_COLOR,								// dest is a vector, src is a color
	UNPACK_TYPE_STRING,										// dest is a char *. unpacker will allocate.
	UNPACK_TYPE_INT,										// dest is an int
	UNPACK_TYPE_FOUR_FLOATS,	 // dest is an array of 4 floats. source is a string like "1 2 3 4"
	UNPACK_TYPE_TWO_FLOATS,		 // dest is an array of 2 floats. source is a string like "1 2"
};

#define UNPACK_FIXED( kname, kdefault, dtype, ofs ) { kname, kdefault, dtype, ofs, 0 }
#define UNPACK_VARIABLE( kname, kdefault, dtype, ofs, sz ) { kname, kdefault, dtype, ofs, sz }
#define UNPACK_END_MARKER { NULL, NULL, UNPACK_TYPE_FLOAT, 0 }

struct KeyValuesUnpackStructure
{
	char const *m_pKeyName;									// null to terminate tbl
	char const *m_pKeyDefault;								// null ok
	KeyValuesUnpackDestinationTypes_t m_eDataType;			// UNPACK_TYPE_INT, ..
	size_t m_nFieldOffset;									// use offsetof to set
	size_t m_nFieldSize;									// for strings or other variable length
};

class IKeyValuesDumpContext
{
public:
	virtual bool KvBeginKey( KeyValues *pKey, int nIndentLevel ) = 0;
	virtual bool KvWriteValue( KeyValues *pValue, int nIndentLevel ) = 0;
	virtual bool KvEndKey( KeyValues *pKey, int nIndentLevel ) = 0;
};

class IKeyValuesDumpContextAsText : public IKeyValuesDumpContext
{
public:
	virtual bool KvBeginKey( KeyValues *pKey, int nIndentLevel );
	virtual bool KvWriteValue( KeyValues *pValue, int nIndentLevel );
	virtual bool KvEndKey( KeyValues *pKey, int nIndentLevel );

public:
	virtual bool KvWriteIndent( int nIndentLevel );
	virtual bool KvWriteText( char const *szText ) = 0;
};

class CKeyValuesDumpContextAsDevMsg : public IKeyValuesDumpContextAsText
{
public:
	// Overrides developer level to dump in DevMsg, zero to dump as Msg
	CKeyValuesDumpContextAsDevMsg( int nDeveloperLevel = 1 );

public:
	virtual bool KvBeginKey( KeyValues *pKey, int nIndentLevel );
	virtual bool KvWriteText( char const *szText );

protected:
	int m_nDeveloperLevel;
};

bool KeyValuesDumpAsDevMsg( KeyValues *pKeyValues, int nIndentLevel = 0, int nDeveloperLevel = 1 );

PLATFORM_OVERLOAD KeyValues *GetGameInfoKey( const char *keyName );
PLATFORM_OVERLOAD bool GetGameInfoBool( const char *keyName, bool defaultValue = false );
PLATFORM_OVERLOAD int GetGameInfoInt( const char *keyName, int defaultValue = 0 );
PLATFORM_OVERLOAD uint64_t GetGameInfoUint64( const char *keyName, uint64 defaultValue = 0 );
PLATFORM_OVERLOAD float GetGameInfoFloat( const char *keyName, float defaultValue = 0.0f );
PLATFORM_OVERLOAD const char *GetGameInfoString( const char *keyName, const char *defaultValue, char *pScratchSpace, size_t nScratchSpaceCount );

#endif