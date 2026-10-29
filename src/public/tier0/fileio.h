#ifndef FILEIO_H
#define FILEIO_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "bufferstring.h"
#include "threadtools.h"
#include "utlbuffer.h"
#include "utlhashtable.h"
#include "utllinkedlist.h"
#include "utlstring.h"
#include "utlvector.h"

#ifndef _WIN32
#include <dirent.h>
#endif

// Windows' FILETIME format on every platform: the number of 100 nanosecond intervals since 1601-01-01 UTC,
// kept as one 64-bit value rather than FILETIME's two 32-bit halves.
typedef uint64 FioFileTime_t;

enum EFioAccessFlags
{
	k_EFioRead = (1 << 0),
	k_EFioWrite = (1 << 1),
	k_EFioReadWrite = k_EFioRead | k_EFioWrite,
};

enum EFioShareFlags
{
	k_EFioShareNone = 0,
	k_EFioShareRead = (1 << 0),
	k_EFioShareWrite = (1 << 1),
	k_EFioShareReadWrite = k_EFioShareRead | k_EFioShareWrite,
	k_EFioShareDelete = (1 << 2),
	k_EFioShareReadDelete = k_EFioShareRead | k_EFioShareDelete,
	k_EFioShareWriteDelete = k_EFioShareWrite | k_EFioShareDelete,
	k_EFioShareReadWriteDelete = k_EFioShareRead | k_EFioShareWrite | k_EFioShareDelete,
};

enum EFioCreateMode
{
	k_EFioOpenExisting = 0,
	k_EFioOpenAlways = 1,
	k_EFioCreateNew = 2,
	k_EFioCreateAlways = 3,
};

enum EFioModifiers
{
	k_EFioNoModifiers = 0,
	k_EFioAsync = (1 << 0),
	k_EFioNoBuffering = (1 << 1),
	k_EFioNoAccessTime = (1 << 2),
	k_EFioSequentialScan = (1 << 3),
	k_EFioTemporary = (1 << 4),
	k_EFioDeleteOnClose = (1 << 5),
};

enum EFioSeekMode
{
	k_EFioSeekSet = 0,
	k_EFioSeekCurrent = 1,
	k_EFioSeekEnd = 2,
};

enum EFioFileInfoFlags
{
	k_EFioFileInfoFile = (1 << 0),
	k_EFioFileInfoDirectory = (1 << 1),
	k_EFioFileInfoSymbolicLink = (1 << 2),
	k_EFioFileInfoReadOnly = (1 << 3),
};

// What to read into a FioBaseFileInfo_t, and what it got
enum EFioMetadataFlags
{
	k_EFioMetadataName = (1 << 0),
	k_EFioMetadataLength = (1 << 1),
	k_EFioMetadataFileFlags = (1 << 2),
	k_EFioMetadataCreateTime = (1 << 3),
	k_EFioMetadataWriteTime = (1 << 4),
	k_EFioMetadataFollowSymbolicLinks = (1 << 5),
	k_EFioMetadataDoesNotExist = (1 << 6),

	k_EFioStandardMetadata = k_EFioMetadataName | k_EFioMetadataLength | k_EFioMetadataFileFlags | k_EFioMetadataCreateTime | k_EFioMetadataWriteTime,
};

// The changes CDirWatcher collects
enum EFioDirWatchFlags
{
	// Files and directories created, deleted or renamed
	k_EFioDirWatchName = (1 << 0),
	k_EFioDirWatchSize = (1 << 1),
	k_EFioDirWatchAttributes = (1 << 2),
	k_EFioDirWatchCreateTime = (1 << 3),
	k_EFioDirWatchWriteTime = (1 << 4),
};

struct FioBaseFileInfo_t
{
	uint64 nLength;
	FioFileTime_t nCreateTime;
	FioFileTime_t nWriteTime;
	// EFioMetadataFlags
	uint32 nValidInfo;
	// EFioFileInfoFlags
	uint32 nFileFlags;
};

struct FioDiskInfo_t
{
	uint64 nTotalBytes;
	uint64 nFreeBytesAvailable;
	uint64 nTotalFreeBytes;
};

//-----------------------------------------------------------------------------
// The functions return 0, or the OS error code: GetLastError() on Windows, errno elsewhere.
//-----------------------------------------------------------------------------

// nModifiers is a combination of EFioModifiers
PLATFORM_OVERLOAD int FioCreateFile( const char *pFileName, EFioAccessFlags eAccess, EFioShareFlags eShare, EFioCreateMode eMode, uint32 nModifiers, OSFILEHANDLE *pHandle );
PLATFORM_OVERLOAD int FioClose( OSFILEHANDLE Handle );
PLATFORM_OVERLOAD int FioCancelIo( OSFILEHANDLE Handle );
PLATFORM_OVERLOAD int FioFlush( OSFILEHANDLE Handle );
PLATFORM_OVERLOAD int FioDuplicate( OSFILEHANDLE Handle, bool bInherit, OSFILEHANDLE *pDuplicate );
PLATFORM_OVERLOAD int FioCreatePipe( OSFILEHANDLE *pRead, OSFILEHANDLE *pWrite, int32 nBufferBytesHint );

PLATFORM_OVERLOAD int FioRead( OSFILEHANDLE Handle, void *pBuffer, int32 nBytes, int32 *pBytesRead );
PLATFORM_OVERLOAD int FioRead( OSFILEHANDLE Handle, void *pBuffer, size_t nBytes, size_t *pBytesRead );
PLATFORM_OVERLOAD int FioReadAt( OSFILEHANDLE Handle, uint64 nFilePos, void *pBuffer, int32 nBytes, int32 *pBytesRead );
PLATFORM_OVERLOAD int FioWrite( OSFILEHANDLE Handle, const void *pBuffer, int32 nBytes, int32 *pBytesWritten );
PLATFORM_OVERLOAD int FioWrite( OSFILEHANDLE Handle, const void *pBuffer, size_t nBytes, size_t *pBytesWritten );
PLATFORM_OVERLOAD int FioWriteAt( OSFILEHANDLE Handle, uint64 nFilePos, const void *pBuffer, int32 nBytes, int32 *pBytesWritten );

PLATFORM_OVERLOAD int FioSeek( OSFILEHANDLE Handle, int64 nOffset, EFioSeekMode eHow, uint64 *pNewPosition );
PLATFORM_OVERLOAD int FioGetSize( OSFILEHANDLE Handle, uint64 *pBytes );
PLATFORM_OVERLOAD int FioSetSize( OSFILEHANDLE Handle, uint64 nBytes );
PLATFORM_OVERLOAD int FioTruncateToSeekPosition( OSFILEHANDLE Handle );

PLATFORM_OVERLOAD int FioGetFileIdentity( OSFILEHANDLE Handle, uint64 *pDiskId, uint64 *pFileId );
PLATFORM_OVERLOAD int FioGetFileIdentity( const char *pFileName, uint64 *pDiskId, uint64 *pFileId );
// Return 0 for the same file
PLATFORM_OVERLOAD int FioIsSameFileIdentity( OSFILEHANDLE Handle1, OSFILEHANDLE Handle2 );
PLATFORM_OVERLOAD int FioIsSameFileIdentity( const char *pFileName1, const char *pFileName2 );

// *pContent is allocated with g_pMemAlloc; files longer than nMaxBytes fail
PLATFORM_OVERLOAD int FioReadFileToMemory( const char *pFileName, void **pContent, int32 *pContentBytes, bool bZeroTerminate, int32 nMaxBytes );
PLATFORM_OVERLOAD int FioReadFileToMemory( const char *pFileName, void **pContent, size_t *pContentBytes, bool bZeroTerminate, size_t nMaxBytes );
PLATFORM_OVERLOAD int FioReadFileToBuffer( const char *pFileName, CUtlBuffer *pBuffer, int32 nMaxBytes );
PLATFORM_OVERLOAD int FioCopyFile( const char *pSourceFileName, const char *pDestFileName, bool bFailIfExists );

PLATFORM_OVERLOAD int FioGetFileCRC32( const char *pFileName, uint32 *pCRC, int32 nBufferBytes, uint64 *pFileBytes );
PLATFORM_OVERLOAD int FioGetFileCRC32Buffer( const char *pFileName, uint32 *pCRC, uint8 *pBuffer, int32 nBufferBytes, uint64 *pFileBytes );

// nMetaFlags is a combination of EFioMetadataFlags
PLATFORM_OVERLOAD int FioGetBaseFileInfo( const char *pFileName, FioBaseFileInfo_t *pInfo, uint32 nMetaFlags );
// *pFileType gets the k_EFioFileInfoFile, k_EFioFileInfoDirectory and k_EFioFileInfoSymbolicLink flags
PLATFORM_INTERFACE int FioGetFileType( const char *pFileName, uint32 nMetaFlags, int *pFileType );
PLATFORM_OVERLOAD int FioGetDiskInfo( const char *pDiskPath, FioDiskInfo_t *pInfo );
PLATFORM_OVERLOAD int FioGetSectorSize( const char *pDiskPath, int *pSectorSize );

PLATFORM_OVERLOAD int FioCreateSymbolicLink( const char *pLinkName, const char *pTargetName, bool bDirectory );
PLATFORM_OVERLOAD int FioReadLink( const char *pLinkName, CBufferString *pTargetBuf, bool bFullPath );

PLATFORM_OVERLOAD bool FioGetActualFilenameCase( const char *pFileName, CBufferString *pActualFileName, bool );
PLATFORM_OVERLOAD const char *FioFindFileInDirCaseInsensitive( const char *pFile, CBufferString *pFileNameOut, bool );

// Is it "." or ".."
PLATFORM_OVERLOAD bool FioIsDotDir( const char *pDir );
// Seconds since 1970, or 0 for earlier times
PLATFORM_OVERLOAD int64 FioFileTimeToTimeT( FioFileTime_t nFioTime );

// Adds the absolute paths of the files under the directory
PLATFORM_OVERLOAD bool FioEnumerateAbsPathsInDirRecursive( const char *pDir, CUtlHashtable< CUtlString > *pAbsPaths );

//-----------------------------------------------------------------------------
// A path in the form the OS takes, built in buffers of a pool
//-----------------------------------------------------------------------------
class CPathString
{
public:
	struct Buffer_t;

	CPathString();
	CPathString( const char *pchUTF8Path );
	CPathString( const char *pchUTF8Dir, const char *pchUTF8File );
	DLL_CLASS_IMPORT ~CPathString();

	// Makes the path absolute
	DLL_CLASS_IMPORT void Set( const char *pchUTF8Path );
	DLL_CLASS_IMPORT void Set( const char *pchUTF8Dir, const char *pchUTF8File );

	const char *GetUTF8Path() const;

#ifdef PLATFORM_WINDOWS
	// The path for the wide Windows functions, prepended with "\\?\" when it's longer than MAX_PATH
	const wchar_t *GetWCharPathPrePended() { if ( !m_pwchWideCharPathPrepended ) PopulateWCharPath(); return m_pwchWideCharPathPrepended; }
#endif

	DLL_CLASS_IMPORT static Buffer_t *NewPathStringBuffer();
	DLL_CLASS_IMPORT static void DeletePathStringBuffer( Buffer_t *pBuffer );

protected:
	DLL_CLASS_IMPORT void PopulateWCharPath();
	DLL_CLASS_IMPORT void DeleteWCharPath();
	DLL_CLASS_IMPORT void Reset();

private:
	CPathString( const CPathString & );
	CPathString &operator=( const CPathString & );

	char *m_pchUTF8Path;
	wchar_t *m_pwchWideCharPathPrepended;
};

//-----------------------------------------------------------------------------
// Iterates the files of a directory:
//	for ( CDirIterator it( "dir/*.cfg" ); it.NextFile(); )
//		it.CurrentFileName();
//-----------------------------------------------------------------------------
class CDirIterator
{
public:
	DLL_CLASS_IMPORT CDirIterator();
	// nMetaFlags tells which EFioMetadataFlags to read into CurrentFileInfo
	DLL_CLASS_IMPORT CDirIterator( const char *pPathAndPattern, uint32 nMetaFlags = k_EFioStandardMetadata );
	DLL_CLASS_IMPORT CDirIterator( const char *pPath, const char *pPattern, uint32 nMetaFlags = k_EFioStandardMetadata );
	DLL_CLASS_IMPORT ~CDirIterator();

	bool Init( const char *pPath, const char *pPattern, uint32 nMetaFlags = k_EFioStandardMetadata );
	bool Init( const char *pPathAndPattern, uint32 nMetaFlags = k_EFioStandardMetadata );
	DLL_CLASS_IMPORT void Stop();

	// Moves to the next file, starting with the first one
	DLL_CLASS_IMPORT bool NextFile();

#ifdef PLATFORM_WINDOWS
	const char *CurrentFileName() const { return m_pszCurrentFileName; }
#else
	const char *CurrentFileName() const { return m_pNames[ m_nCurName ]->d_name; }
#endif
	DLL_CLASS_IMPORT const char *CurrentFileFullPath();

	uint32 CurrentFileInfoMetaFlags() const;
	const FioBaseFileInfo_t &CurrentFileInfo() const;
	uint64 CurrentFileLength() const;
	FioFileTime_t CurrentFileCreateTime() const;
	FioFileTime_t CurrentFileWriteTime() const;
	uint32 CurrentFileFlags() const;
	bool CurrentIsDir() const;
	bool CurrentIsDotDir() const;
	bool CurrentIsReadOnly() const;
	bool CurrentIsSymbolicLink() const;

protected:
	DLL_CLASS_IMPORT void Construct();
	DLL_CLASS_IMPORT bool Init( const char *pPath, const char *pPattern, const char *pPathAndPattern, uint32 nMetaFlags );
	DLL_CLASS_IMPORT bool SplitPathAndPattern( const char *pPathAndPattern, CUtlString *pPathStr, CUtlString *pPatternStr );

	// EFioMetadataFlags
	uint32 m_nMetaFlags;
	bool m_bNoFiles;
	bool m_bHaveFullPath;
	CUtlString m_pBasePath;
	char *m_pFullPath;
	int m_nFullPathChars;
	FioBaseFileInfo_t m_CurInfo;
	bool m_bUsedFirstFile;
#ifdef PLATFORM_WINDOWS
	HANDLE m_hFind;
	char *m_pszCurrentFileName;
	int m_nCurrentFileNameBytes;
#else
	struct dirent **m_pNames;
	int m_nNames;
	int m_nCurName;
#endif

private:
	CDirIterator( const CDirIterator & );
	CDirIterator &operator=( const CDirIterator & );
};

//-----------------------------------------------------------------------------
// Collects the files that change in a directory:
//	CDirWatcher watcher;
//	watcher.SetDirToWatch( "cfg", k_EFioDirWatchName | k_EFioDirWatchWriteTime, true );
//	...
//	CDirTreeWatchManager::CheckForPendingChanges();
//	for ( CBufferString file; watcher.GetChangedFile( &file ); )
//		file.Get();
// Only Windows reports changes; on other platforms SetDirToWatch just sets the directory.
//-----------------------------------------------------------------------------
class CDirWatcher
{
public:
	DLL_CLASS_IMPORT CDirWatcher();
	~CDirWatcher();

	// nChangeFlags is a combination of EFioDirWatchFlags.
	// Changes are delivered to the monitoring thread, the calling one by default, which checks them when it's alertable.
	// nBufferBytes is 16KB when 0.
#ifdef PLATFORM_WINDOWS
	DLL_CLASS_IMPORT bool SetDirToWatch( const char *pchDir, uint32 nChangeFlags, bool bWatchSubDirsIfPossible, ThreadId_t nMonitoringThreadId = 0, size_t nBufferBytes = 0 );
#else
	DLL_CLASS_IMPORT bool SetDirToWatch( const char *pchDir, uint32 nChangeFlags, bool bWatchSubDirsIfPossible, void *pMonitoringThread = nullptr, size_t nBufferBytes = 0 );
#endif
	DLL_CLASS_IMPORT void Stop();

	// Gets the full path of a changed file and removes it from the list
	DLL_CLASS_IMPORT bool GetChangedFile( CBufferString *pFile );
	// Appends the changed files to buf and points pFiles at them, emptying the list, and returns how many there were
	DLL_CLASS_IMPORT uint32 GetChangedFiles( CUtlBuffer *pBuf, CUtlVector< char * > *pFiles );

	const char *GetDirName() const;

private:
	DLL_CLASS_IMPORT void AddFileToChangeList( const char *pchFile );

	CDirWatcher( const CDirWatcher & );
	CDirWatcher &operator=( const CDirWatcher & );

	CUtlString m_BaseDir;
	CUtlLinkedList< CUtlString, uint32 > m_listChangedFiles;
	// The directory's HANDLE on Windows
	void *m_hFile;
	// An OVERLAPPED followed by a pointer to this watcher
	void *m_pOverlapped;
	// The buffer of m_nBufferBytes that ReadDirectoryChangesW fills with FILE_NOTIFY_INFORMATION records
	void *m_pFileInfo;
	size_t m_nBufferBytes;
	bool m_bWatchSubDirsIfPossible;
	// The EFioDirWatchFlags as Windows' FILE_NOTIFY_CHANGE_* flags
	uint32 m_nNotifyFilter;
};

//-----------------------------------------------------------------------------
// Watches directory trees for changed files
//-----------------------------------------------------------------------------
class CDirTreeWatchManager
{
public:
	// Walks a directory tree: BeginTree, then VisitFile with each file and EnterDirectory and LeaveDirectory
	// around each subdirectory, then EndTree. The names are relative to the directory entered last.
	class Visitor
	{
	public:
		virtual void VisitFile( const char *pFileName, const FioBaseFileInfo_t *pFileInfo ) = 0;
		virtual void EnterDirectory( const char *pDirName );
		virtual void LeaveDirectory();
		virtual void BeginTree( const char *pRootDir );
		virtual void EndTree();
	};

	// nMetaFlags tells which EFioMetadataFlags GetChangedFile reads
	DLL_CLASS_IMPORT CDirTreeWatchManager( uint32 nMetaFlags );
	DLL_CLASS_IMPORT ~CDirTreeWatchManager();

	// Walks the files already in the directory with pVisitor, when given
	DLL_CLASS_IMPORT bool AddWatch( const char *pDir, Visitor *pVisitor, size_t nBufferBytes = 0 );
	DLL_CLASS_IMPORT void Stop();

	DLL_CLASS_IMPORT bool GetChangedFile( CBufferString *pFile, FioBaseFileInfo_t *pFileInfo );
	// Walks the changed files of each watch with pVisitor; a deleted file has k_EFioMetadataDoesNotExist in its nValidInfo
	DLL_CLASS_IMPORT void TraverseChangedFiles( Visitor *pVisitor );

#ifdef PLATFORM_WINDOWS
	DLL_CLASS_IMPORT void SetMonitoringThreadInfo( ThreadId_t nMonitoringThreadId );
#else
	DLL_CLASS_IMPORT void SetMonitoringThreadInfo( void *pMonitoringThread );
#endif

	// Lets the watches of the calling thread get their changes
	DLL_CLASS_IMPORT static void CheckForPendingChanges();

private:
	// The watches form a list. A symbolic link found in a watched tree gets a watch on its target,
	// whose changed paths are reported under the link.
	struct Watcher_t
	{
		Watcher_t *m_pNext;
		CDirWatcher m_Watch;
		// The link's target and the link
		CUtlString m_LinkRemapFrom;
		CUtlString m_LinkRemapTo;
	};

	CDirTreeWatchManager( const CDirTreeWatchManager & );
	CDirTreeWatchManager &operator=( const CDirTreeWatchManager & );

	// EFioMetadataFlags
	uint32 m_nMetaFlags;
	Watcher_t *m_pWatchers;
#ifdef PLATFORM_WINDOWS
	ThreadId_t m_nMonitoringThreadId;
#else
	void *m_pMonitoringThread;
#endif
};

#endif // FILEIO_H
