#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "public/appframework/iappsystem.h"
#include "public/tier0/bufferstring.h"
#include "public/tier0/checksum_crc.h"
#include "public/tier0/platform.h"
#include "public/tier0/threadtools.h"
#include "public/tier0/utlstring.h"
#include "public/tier0/utlstringtoken.h"
#include "public/tier0/utlvector.h"
#include "filenamesymboltable.h"
#include "public/steamworks/isteamugc.h"

class CUtlBuffer;
class KeyValues;
class IFileList;
struct SearchPathStateHandle_t;
class ILowLevelFileIO;
class CSteamID;
class CPackedStore;
class IFileSystem;

typedef void * FileHandle_t;
struct FileFindHandle_t
{
	IFileSystem *m_pFileSystem;
	uintptr_t m_nFindDataIndex;
};
typedef void (*FileSystemLoggingFunc_t)( const char *fileName, const char *accessType );
typedef int WaitForResourcesHandle_t;

#define FILESYSTEM_MAX_SEARCH_PATHS 128

enum FileSystemSeek_t
{
	FILESYSTEM_SEEK_HEAD	= SEEK_SET,
	FILESYSTEM_SEEK_CURRENT = SEEK_CUR,
	FILESYSTEM_SEEK_TAIL	= SEEK_END,
};

enum
{
	FILESYSTEM_INVALID_FIND_HANDLE = -1
};

enum FileWarningLevel_t
{
	// A problem!
	FILESYSTEM_WARNING = -1,

	// Don't print anything
	FILESYSTEM_WARNING_QUIET = 0,

	// On shutdown, report names of files left unclosed
	FILESYSTEM_WARNING_REPORTUNCLOSED,

	// Report number of times a file was opened, closed
	FILESYSTEM_WARNING_REPORTUSAGE,

	// Report all open/close events to console ( !slow! )
	FILESYSTEM_WARNING_REPORTALLACCESSES,

	// Report all open/close/read events to the console ( !slower! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_READ,

	// Report all open/close/read/write events to the console ( !slower! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_READWRITE,

	// Report all open/close/read/write events and all async I/O file events to the console ( !slower(est)! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_ASYNC,

};

// search path filtering
enum PathTypeFilter_t
{
	FILTER_NONE          = 0,	// no filtering, all search path types match
	FILTER_CULLLOCALIZED = 1,
	FILTER_CULLPACK      = 2,	// pack based search paths are culled (maps and zips)
	FILTER_CULLNONPACK   = 3,	// non-pack based search paths are culled
};

// search path querying (bit flags)
enum
{
	PATH_IS_NORMAL      = 0x00, // normal path, not pack based
	PATH_IS_PACKFILE    = 0x01, // path is a pack file
	PATH_IS_MAPPACKFILE = 0x02, // path is a map pack file
	PATH_IS_DVDDEV		= 0x04, // path is the dvddev cache
};
typedef uint32_t PathTypeQuery_t;

#define IS_PACKFILE( n ) ( n & ( PATH_IS_PACKFILE | PATH_IS_MAPPACKFILE ) )
#define IS_DVDDEV( n )   ( n & PATH_IS_DVDDEV )

enum DVDMode_t
{
	DVDMODE_OFF    = 0, // not using dvd
	DVDMODE_STRICT = 1, // dvd device only
	DVDMODE_DEV    = 2, // dev mode, mutiple devices ok
	DVDMODE_DEV_VISTA = 3, // dev mode from a vista host, mutiple devices ok
};

enum FilesystemMountRetval_t
{
	FILESYSTEM_MOUNT_OK = 0,
	FILESYSTEM_MOUNT_FAILED,
};

enum SearchPathAdd_t
{
	PATH_ADD_TO_HEAD,			// First path searched
	PATH_ADD_TO_TAIL,			// Last path searched
	PATH_ADD_TO_TAIL_ATINDEX,	// First path searched
};

enum SearchPathPriority_t
{
	SEARCH_PATH_PRIORITY_DEFAULT     = 0,
	SEARCH_PATH_PRIORITY_LOOSE_FILES = 1,
	SEARCH_PATH_PRIORITY_VPK         = 2,
};

enum GetSearchPathTypes_t
{
	GET_SEARCH_PATH_NO_PACK_FILES   = 0,
	GET_SEARCH_PATH_NO_AUTO_MOUNTED = 1,
	GET_SEARCH_PATH_ALL             = 2,
};

enum FilesystemOpenExFlags_t
{
	FSOPEN_UNBUFFERED		= (1 << 0),
	FSOPEN_FORCE_TRACK_CRC	= (1 << 1),		// This makes it calculate a CRC for the file (if the file came from disk) regardless 
											// of the IFileList passed to RegisterFileWhitelist.
	FSOPEN_NEVERINPACK	    = (1 << 2),		// 360 only, hint to FS that file is not allowed to be in pack file
};

#define FILESYSTEM_INVALID_HANDLE	( FileHandle_t )0

struct FileSystemStatistics
{
	CInterlockedUInt	nReads,		
						nWrites,		
						nBytesRead,
						nBytesWritten,
						nSeeks;
};

//-----------------------------------------------------------------------------
// File system allocation functions. Client must free on failure
//-----------------------------------------------------------------------------
typedef void *(*FSAllocFunc_t)( const char *pszFilename, unsigned nBytes );


//-----------------------------------------------------------------------------
// Used to display dirty disk error functions
//-----------------------------------------------------------------------------
typedef void (*FSDirtyDiskReportFunc_t)();


//-----------------------------------------------------------------------------
// Asynchronous support types
//-----------------------------------------------------------------------------
typedef void* FSAsyncControl_t;
typedef void* FSAsyncFile_t;
const FSAsyncFile_t FS_INVALID_ASYNC_FILE = (FSAsyncFile_t)(0x0000ffff);

enum FSAsyncStatus_t
{
	FSASYNC_ERR_ALIGNMENT    = -6,	// read parameters invalid for unbuffered IO
	FSASYNC_ERR_FAILURE      = -5,	// hard subsystem failure
	FSASYNC_ERR_READING      = -4,	// read error on file
	FSASYNC_ERR_NOMEMORY     = -3,	// out of memory for file read
	FSASYNC_ERR_UNKNOWNID    = -2,	// caller's provided id is not recognized
	FSASYNC_ERR_FILEOPEN     = -1,	// filename could not be opened (bad path, not exist, etc)
	FSASYNC_OK               = 0,	// operation is successful
	FSASYNC_STATUS_PENDING,			// file is properly queued, waiting for service
	FSASYNC_STATUS_INPROGRESS,		// file is being accessed
	FSASYNC_STATUS_ABORTED,			// file was aborted by caller
	FSASYNC_STATUS_UNSERVICED,		// file is not yet queued
};

//---------------------------------------------------------
// Async request flags
//---------------------------------------------------------
enum FSAsyncFlags_t
{
	FSASYNC_FLAGS_ALLOCNOFREE		= ( 1 << 0 ),	// do the allocation for dataPtr, but don't free
	FSASYNC_FLAGS_FREEDATAPTR		= ( 1 << 1 ),	// free the memory for the dataPtr post callback
	FSASYNC_FLAGS_SYNC				= ( 1 << 2 ),	// Actually perform the operation synchronously. Used to simplify client code paths
	FSASYNC_FLAGS_NULLTERMINATE		= ( 1 << 3 ),	// allocate an extra byte and null terminate the buffer read in
};

//---------------------------------------------------------
// Return value for CheckFileCRC.
//---------------------------------------------------------
enum EFileCRCStatus
{
	k_eFileCRCStatus_CantOpenFile,		// We don't have this file. 
	k_eFileCRCStatus_GotCRC
};

// Used in CacheFileCRCs.
enum ECacheCRCType
{
	k_eCacheCRCType_SingleFile,
	k_eCacheCRCType_Directory,
	k_eCacheCRCType_Directory_Recursive
};

struct FileAsyncRequest_t;
typedef void (*FSAsyncCallbackFunc_t)(const FileAsyncRequest_t &request, int nBytesRead, FSAsyncStatus_t err);

//-----------------------------------------------------------------------------
// Used to add results from async directory scans
//-----------------------------------------------------------------------------
typedef void (*FSAsyncScanAddFunc_t)( void* pContext, char* pFoundPath, char* pFoundFile );
typedef void (*FSAsyncScanCompleteFunc_t)( void* pContext, FSAsyncStatus_t err );

struct FileAsyncRequest_t
{
	FileAsyncRequest_t()	{ memset( this, 0, sizeof(*this) ); hSpecificAsyncFile = FS_INVALID_ASYNC_FILE;	}
	const char *			pszFilename;		// file system name
	void *					pData;				// optional, system will alloc/free if NULL
	int						nOffset;			// optional initial seek_set, 0=beginning
	int						nBytes;				// optional read clamp, -1=exist test, 0=full read
	FSAsyncCallbackFunc_t	pfnCallback;		// optional completion callback
	void *					pContext;			// caller's unique file identifier
	int						priority;			// inter list priority, 0=lowest
	unsigned				flags;				// behavior modifier
	const char *			pszPathID;			// path ID (NOTE: this field is here to remain binary compatible with release HL2 filesystem interface)
	FSAsyncFile_t			hSpecificAsyncFile; // Optional hint obtained using AsyncBeginRead()
	FSAllocFunc_t			pfnAlloc;			// custom allocator. can be null. not compatible with FSASYNC_FLAGS_FREEDATAPTR
};


class CUnverifiedCRCFile
{
public:
	char m_PathID[MAX_PATH];
	char m_Filename[MAX_PATH];
	CRC32_t m_CRC;
};


// Spew flags for SetWhitelistSpewFlags (set with the fs_whitelist_spew_flags cvar).
// Update the comment for the fs_whitelist_spew_flags cvar if you change these.
#define WHITELIST_SPEW_WHILE_LOADING		0x0001	// list files as they are added to the CRC tracker
#define WHITELIST_SPEW_RELOAD_FILES			0x0002	// show files the filesystem is telling the engine to reload
#define WHITELIST_SPEW_DONT_RELOAD_FILES	0x0004	// show files the filesystem is NOT telling the engine to reload



// DLC license mask flags is 32 publisher defined bits
// MSW 16 bits in 8.8: Type.SubVersion
// LSW 16 bits: Flags

// return id component
#define DLC_LICENSE_ID( x )				( ( ( (unsigned int)( x ) ) >> 24 ) & 0x000000FF )
// returns minor version component (not generally used, i.e. we dont rev dlc's yet)
#define DLC_LICENSE_MINORVERSION( x )	( ( ( (unsigned int)( x ) ) >> 16 ) & 0x000000FF )
// returns license flags
#define DLC_LICENSE_FLAGS( x )			( ( ( (unsigned int)( x ) ) & 0x0000FFFF ) )

#define DLCFLAGS_PRESENCE_ONLY			0x0001	// causes no search path loadout


enum MemoryMappedFileFlags_t
{
	MMF_READ = (1 << 0),
	MMF_WRITE = (1 << 1),
};

class IMemoryMappedFile
{
public:
	IMemoryMappedFile() : m_pMappedMemory( NULL ), m_nMappedSize( 0 ), m_nFlags( (MemoryMappedFileFlags_t)0 ), m_nRefCount( 1 ) {}
	virtual ~IMemoryMappedFile() {}

	uint32_t AddRef() { return ++m_nRefCount; }
	uint32_t Release()
	{
		uint32_t nRefCount = --m_nRefCount;
		if ( nRefCount == 0 )
			ReachedZeroReferences();
		return nRefCount;
	}

	const void *GetMappedMemory() const { return m_pMappedMemory; }
	size_t GetMappedSize() const { return m_nMappedSize; }
	MemoryMappedFileFlags_t GetFlags() const { return m_nFlags; }

	virtual bool WriteToContents( size_t nOffset, const void *pData, size_t nSize ) = 0;

protected:
	virtual void ReachedZeroReferences() = 0;

	void *m_pMappedMemory;
	size_t m_nMappedSize;
	MemoryMappedFileFlags_t m_nFlags;
	CInterlockedUInt m_nRefCount;
};

class IVPKSignatureCallbacks
{
public:
	virtual void OnSignatureFailed( CPackedStore *pVPK, uint8_t nReason ) = 0;
	virtual bool ShouldVerifySignature( CPackedStore *pVPK ) = 0;
};

class IFileSystem : public IAppSystem
{
public:
	//--------------------------------------------------------
	// IBaseFileSystem
	//--------------------------------------------------------
	virtual int				Read(void* pOutput, int size, FileHandle_t file) = 0;
	virtual int				Write(void const* pInput, uint64_t size, FileHandle_t file) = 0;

	// if pathID is NULL, all paths will be searched for the file
	virtual FileHandle_t	Open(const char *pFileName, const char *pOptions, const char *pathID = 0) = 0;
	virtual void			Close(FileHandle_t file) = 0;


	virtual void			Seek(FileHandle_t file, int64_t pos, FileSystemSeek_t seekType) = 0;
	virtual int64_t			Tell(FileHandle_t file) = 0;
	virtual int64_t			Size(FileHandle_t file) = 0;
	virtual int64_t			Size(const char *pFileName, const char *pPathID = 0) = 0;

	virtual void			Flush(FileHandle_t file) = 0;
	virtual bool			Precache(const char *pFileName, const char *pPathID = 0) = 0;

	virtual bool			FileExists(const char *pFileName, const char *pPathID = 0) = 0;
	virtual bool			IsFileWritable(char const *pFileName, const char *pPathID = 0) = 0;
	virtual bool			SetFileWritable(char const *pFileName, bool writable, const char *pPathID = 0) = 0;

	virtual int64_t			GetFileTime(const char *pFileName, const char *pPathID = 0) = 0;

	//--------------------------------------------------------
	// Reads/writes files to utlbuffers. Use this for optimal read performance when doing open/read/close
	//--------------------------------------------------------
	virtual bool			ReadFile(const char *pFileName, const char *pPathID, CUtlBuffer &buf, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL) = 0;
	virtual bool			WriteFile(const char *pFileName, const char *pPathID, CUtlBuffer &buf) = 0;
	virtual bool			UnzipFile(const char *pFileName, const char *pPathID, const char *pDestination) = 0;
	virtual bool			CopyAFile(const char *pFileName, const char *pPathID, const char *pDestination, bool bDontOverwrite = false) = 0;

	// Returns NULL on failure, Release() the result when done
	virtual IMemoryMappedFile *CreateMemoryMap( FileHandle_t hFile, MemoryMappedFileFlags_t nFlags ) = 0;
	virtual IMemoryMappedFile *CreateMemoryMap( const char *pFileName, const char *pPathID, MemoryMappedFileFlags_t nFlags ) = 0;

	//--------------------------------------------------------
	// Search path manipulation
	//--------------------------------------------------------

	// Add paths in priority order (mod dir, game dir, ....)
	// If one or more .pak files are in the specified directory, then they are
	//  added after the file system path
	// If the path is the relative path to a .bsp file, then any previous .bsp file 
	//  override is cleared and the current .bsp is searched for an embedded PAK file
	//  and this file becomes the highest priority search path ( i.e., it's looked at first
	//   even before the mod's file system path ).
	virtual void			AddSearchPath( const char *pPath, const char *pathID, SearchPathAdd_t addType = PATH_ADD_TO_TAIL, SearchPathPriority_t priority = SEARCH_PATH_PRIORITY_DEFAULT, CUtlStringToken pathMergeToken = CUtlStringToken() ) = 0;
	virtual bool			RemoveSearchPath( const char *pPath, const char *pathID = 0 ) = 0;
	
	virtual SearchPathStateHandle_t		*SaveSearchPathState( const char *pszName ) const = 0;
	virtual void			RestoreSearchPathState( SearchPathStateHandle_t *pState ) = 0;
	virtual void			DestroySearchPathState( SearchPathStateHandle_t *pState ) = 0;

	// Remove all search paths (including write path?)
	virtual void			RemoveAllSearchPaths( void ) = 0;
	
	// Remove search paths associated with a given pathID
	virtual void			RemoveSearchPaths( const char *szPathID ) = 0;

	// This is for optimization. If you mark a path ID as "by request only", then files inside it
	// will only be accessed if the path ID is specifically requested. Otherwise, it will be ignored.
	// If there are currently no search paths with the specified path ID, then it will still
	// remember it in case you add search paths with this path ID.
	virtual void			MarkPathIDByRequestOnly( const char *pPathID, bool bRequestOnly ) = 0;

	virtual bool			IsFileInReadOnlySearchPath( const char *pFileName, const char *pPathID ) = 0;
	virtual void			SetSearchPathReadOnly( const char *pPath, const char *pPathID, bool bReadOnly ) = 0;

	// converts a partial path into a full path
	virtual const char		*RelativePathToFullPath( const char *pFileName, const char *pPathID, CBufferString &pLocalPath, PathTypeFilter_t pathFilter = FILTER_NONE, PathTypeQuery_t *pPathType = NULL ) = 0;

	virtual bool			RelativePathToFullWritePath( const char *pRelativePath, const char *pWritePathID, CBufferString &pLocalPath ) = 0;

	// Returns the nSearchPathsToGet amount of paths, each path is separated by ;s. Returns true if pathID has any paths
	virtual bool			GetSearchPath( const char *pathID, GetSearchPathTypes_t pathType, CBufferString &pPath, int nSearchPathsToGet ) = 0;
	
	// Returns the merge token of the search path that holds pPath, or an empty token
	virtual CUtlStringToken	GetMergeTokenForPath( const char *pPath, const char *pPathID ) = 0;
	// Writes the paths merged under the token to pPath, returns its length plus one
	virtual int				GetMergedPaths( CUtlStringToken pathMergeToken, CBufferString *pPath ) = 0;

	//--------------------------------------------------------
	// File manipulation operations
	//--------------------------------------------------------

	// Deletes a file (on the WritePath)
	virtual bool			RemoveFile( char const* pRelativePath, const char *pathID = 0 ) = 0;

	// Renames a file (on the WritePath)
	virtual bool			RenameFile( char const *pOldPath, char const *pNewPath, const char *pathID = 0 ) = 0;

	// create a local directory structure
	virtual bool			CreateDirHierarchy( const char *path, const char *pathID = 0 ) = 0;
	virtual bool			CreateDirHierarchyForFile( const char *pFileName, const char *pathID ) = 0;

	// File I/O and info
	virtual bool			IsDirectory( const char *pFileName, const char *pathID = 0 ) = 0;

	virtual void			FileTimeToString( int64_t fileTime, CBufferString &out ) = 0;

	//--------------------------------------------------------
	// Open file operations
	//--------------------------------------------------------

	virtual void			SetBufferSize( FileHandle_t file, unsigned nBytes ) = 0;

	virtual bool			IsOk( FileHandle_t file ) = 0;

	virtual bool			EndOfFile( FileHandle_t file ) = 0;
	
	virtual char			*ReadLine( char *pOutput, int maxChars, FileHandle_t file ) = 0;

	virtual int				FPrintf( FileHandle_t file, const char *pFormat, ... ) FMTFUNCTION( 3, 4 ) = 0;

	//--------------------------------------------------------
	// Dynamic library operations
	//--------------------------------------------------------

	// load/unload modules
	virtual void 		*LoadModule( const char *pFileName, const char *pPathID = 0, bool bValidatedDllOnly = true ) = 0;
	virtual void			UnloadModule( void *pModule ) = 0;

	//--------------------------------------------------------
	// File searching operations
	//--------------------------------------------------------

	// FindFirst/FindNext. Also see FindFirstEx.
	virtual const char		*FindFirst( const char *pWildCard, FileFindHandle_t *pHandle ) = 0;
	virtual const char		*FindNext( FileFindHandle_t &handle ) = 0;
	virtual bool			FindIsDirectory( FileFindHandle_t &handle ) = 0;
	virtual void			FindClose( FileFindHandle_t &handle ) = 0;

	// Same as FindFirst, but you can filter by path ID, which can make it faster.
	virtual const char		*FindFirstEx( const char *pWildCard, const char *pPathID, FileFindHandle_t *pHandle ) = 0;

	virtual void			FindFileAbsoluteList( CUtlVector<CUtlString> &output, const char *pWildCard, const char *pPathID ) = 0;

	//--------------------------------------------------------
	// File name and directory operations
	//--------------------------------------------------------

	// Returns true on success ( based on current list of search paths, otherwise false if 
	//  it can't be resolved )
	virtual bool			FullPathToRelativePath( const char *pFullpath, const char *pPathID, CBufferString &pRelative ) = 0;

	// Gets the current working directory
	virtual bool			GetCurrentDirectory( CBufferString &pDirectory ) = 0;

	//--------------------------------------------------------
	// Filename dictionary operations
	//--------------------------------------------------------

	virtual FileNameHandle_t	FindOrAddFileName( char const *pFileName ) = 0;
	virtual bool				String( const FileNameHandle_t& handle, CBufferString &buf ) = 0;

	virtual void			Trace_DumpUnclosedFiles() = 0;

	virtual void			PrintSearchPaths( void ) = 0;

	// output
	virtual void			SetWarningFunc( void (*pfnWarning)( const char *fmt, ... ) ) = 0;
	virtual void			SetWarningLevel( FileWarningLevel_t level ) = 0;

	// Returns the file system statistics retreived by the implementation.  Returns NULL if not supported.
	virtual const FileSystemStatistics *GetFilesystemStatistics() = 0;

	virtual FileHandle_t	OpenEx( const char *pFileName, const char *pOptions, unsigned flags = 0, const char *pathID = 0 ) = 0;

	// Extended version of read provides more context to allow for more optimal reading
	virtual int				ReadEx( void* pOutput, int sizeDest, int size, FileHandle_t file ) = 0;
	virtual int				ReadFileEx( const char *pFileName, const char *pPath, void **ppBuf, bool bNullTerminate = false, bool bOptimalAlloc = false, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL ) = 0;

	virtual FileNameHandle_t	FindFileName( char const *pFileName ) = 0;

	// Fixme, we could do these via a string embedded into the compiled data, etc...
	enum KeyValuesPreloadType_t
	{
		TYPE_VMT,
		TYPE_SOUNDEMITTER,
		TYPE_SOUNDSCAPE,
		TYPE_SOUNDOPERATORS,
		NUM_PRELOAD_TYPES
	};

	// If the "PreloadedData" hasn't been purged, then this'll try and instance the KeyValues using the fast path of compiled keyvalues loaded during startup.
	// Otherwise, it'll just fall through to the regular KeyValues loading routines
	virtual KeyValues	*LoadKeyValues( KeyValuesPreloadType_t type, char const *filename, char const *pPathID = 0 ) = 0;
	virtual bool		LoadKeyValues( KeyValues& head, KeyValuesPreloadType_t type, char const *filename, char const *pPathID = 0 ) = 0;

	virtual bool			GetFileTypeForFullPath( char const *pFullPath, CBufferString &buf ) = 0;

	//--------------------------------------------------------
	//--------------------------------------------------------
	virtual bool		ReadToBuffer( FileHandle_t hFile, CUtlBuffer &buf, int nMaxBytes = 0, FSAllocFunc_t pfnAlloc = NULL ) = 0;

	//--------------------------------------------------------
	// Optimal IO operations
	//--------------------------------------------------------
	virtual bool		GetOptimalIOConstraints( FileHandle_t hFile, unsigned *pOffsetAlign, unsigned *pSizeAlign, unsigned *pBufferAlign ) = 0;
	virtual void		*AllocOptimalReadBuffer( FileHandle_t hFile, unsigned nSize = 0, unsigned nOffset = 0 ) = 0;
	virtual void		FreeOptimalReadBuffer( void *p ) = 0;

	//--------------------------------------------------------
	//
	//--------------------------------------------------------
	virtual int			GetPathIndex( const FileNameHandle_t &handle ) = 0;
	virtual int64		GetPathTime( const char *pPath, const char *pPathID ) = 0;

	virtual void		GetSearchPathID( CBufferString &inout ) = 0;

	// File path is "ugc:<UGCHandle_t value>"
	virtual void AddUGCVPKFile( const char *pszName, const char *pPathID, SearchPathAdd_t addType = PATH_ADD_TO_TAIL ) = 0;
	virtual void RemoveUGCVPKFile( const char *pszName ) = 0;
	virtual bool IsUGCVPKFileLoaded( const char *pszName ) = 0;

	virtual void EnableAutoVPKFileLoading( bool bEnable ) = 0;
	virtual bool GetAutoVPKFileLoading( void ) = 0;
	virtual bool ValidateLoadedVPKs( void ) = 0;

	virtual void LoadUGC( UGCHandle_t ugcId, const char *pFilename, const char *pPathID ) = 0;
	virtual void UnloadUGC( UGCHandle_t ugcId, const char *pPathID ) = 0;
	virtual bool IsUGCLoaded( UGCHandle_t ugcId ) const = 0;

	virtual UGCHandle_t		ParseUGCHandleFromFilename( const char *pFileName, char **ppRemainingFilename = nullptr ) const = 0;
	virtual void			CreateFilenameForUGCFile( CBufferString &out, UGCHandle_t ugcId, const char *pExtension, char separator = CORRECT_PATH_SEPARATOR ) const = 0;
	virtual FileHandle_t	OpenUGCFile( UGCHandle_t ugcId ) = 0;

	virtual void            SetIODelayAlarm( float flThreshhold ) = 0;
	
	virtual bool			DeleteDirectory( const char *pFileName, const char *pathID = 0 ) = 0;
	virtual bool			DeleteDirectoryAndContents_R( const char *pFileName, const char *pathID, bool bForce ) = 0;
	
	virtual bool			IsPathInvalidForFilesystem( const char *pFileName ) = 0;
	
	virtual void			GetAvailableDrives( CUtlVector<CUtlString> &drives ) = 0;

	virtual CUtlString		ReadLine( FileHandle_t file, bool bStripNewline = true ) = 0;

	virtual void			GetSearchPathsForPathID( const char *pathID, GetSearchPathTypes_t nPathTypes, CUtlVector<CUtlString> &list ) = 0;

	virtual bool			MarkContentCorrupt( bool bMissingFilesOnly, const char *pFile, const char *pReason ) = 0;
	virtual void			SetContentCorruptionReportingEnabled( bool bEnabled ) = 0;

	// Nests, every true call needs a matching false one
	virtual void			SuppressAsyncFileRequestFlushOnPathChange( bool bEnable ) = 0;

	enum AccesLogWriteResult_t
	{
		ACCESS_LOG_WRITE_NO_LOG,
		ACCESS_LOG_WRITE_FAILED,
		ACCESS_LOG_WRITE_SUCCESS,
	};

	virtual AccesLogWriteResult_t WriteFileAccessLog( const char *pHeader, const char *pFileName, const char *pPath, bool bClearLog ) = 0;

	virtual void			SetVPKSignatureVerification( const void *pKey, int nKeySize, IVPKSignatureCallbacks *pCallbacks ) = 0;
	virtual void			MarkVPKAsUntrustedAddon( const char *pVPKPath ) = 0;
	virtual bool			IsFileInUntrustedAddon( const char *pPath ) = 0;
};

#endif