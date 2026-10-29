#include "public/tier0/fileio.h"

CPathString::CPathString()
: m_pchUTF8Path( nullptr ), m_pwchWideCharPathPrepended( nullptr )
{}

CPathString::CPathString( const char *pchUTF8Path )
: CPathString()
{ Set( pchUTF8Path ); }

CPathString::CPathString( const char *pchUTF8Dir, const char *pchUTF8File )
: CPathString()
{ Set( pchUTF8Dir, pchUTF8File ); }

const char * CPathString::GetUTF8Path() const
{ return m_pchUTF8Path; }

bool CDirIterator::Init( const char *pPath, const char *pPattern, uint32 nMetaFlags )
{ return Init( pPath, pPattern, nullptr, nMetaFlags ); }

bool CDirIterator::Init( const char *pPathAndPattern, uint32 nMetaFlags )
{ return Init( nullptr, nullptr, pPathAndPattern, nMetaFlags ); }

uint32 CDirIterator::CurrentFileInfoMetaFlags() const
{ return m_CurInfo.nValidInfo; }

const FioBaseFileInfo_t & CDirIterator::CurrentFileInfo() const
{ return m_CurInfo; }

uint64 CDirIterator::CurrentFileLength() const
{ return m_CurInfo.nLength; }

FioFileTime_t CDirIterator::CurrentFileCreateTime() const
{ return m_CurInfo.nCreateTime; }

FioFileTime_t CDirIterator::CurrentFileWriteTime() const
{ return m_CurInfo.nWriteTime; }

uint32 CDirIterator::CurrentFileFlags() const
{ return m_CurInfo.nFileFlags; }

bool CDirIterator::CurrentIsDir() const
{ return ( m_CurInfo.nFileFlags & k_EFioFileInfoDirectory ) != 0; }

bool CDirIterator::CurrentIsDotDir() const
{ return FioIsDotDir( CurrentFileName() ); }

bool CDirIterator::CurrentIsReadOnly() const
{ return ( m_CurInfo.nFileFlags & k_EFioFileInfoReadOnly ) != 0; }

bool CDirIterator::CurrentIsSymbolicLink() const
{ return ( m_CurInfo.nFileFlags & k_EFioFileInfoSymbolicLink ) != 0; }

CDirWatcher::~CDirWatcher()
{ Stop(); }

const char * CDirWatcher::GetDirName() const
{ return m_BaseDir.Get(); }

void CDirTreeWatchManager::Visitor::EnterDirectory( const char *pDirName )
{}

void CDirTreeWatchManager::Visitor::LeaveDirectory()
{}

void CDirTreeWatchManager::Visitor::BeginTree( const char *pRootDir )
{}

void CDirTreeWatchManager::Visitor::EndTree()
{}
