#include "public/filesystem/iasyncfilesystem.h"

IIOCompletionQueue::~IIOCompletionQueue()
{}

IAsyncReadRequest2::IAsyncReadRequest2( const char *pFileName, const char *pPathID )
: m_fileName( pFileName ), m_szPathID( pPathID )
{}

IAsyncReadRequest2::~IAsyncReadRequest2()
{}

const char * IAsyncReadRequest2::GetFileName() const
{ return m_fileName.Get(); }

const char * IAsyncReadRequest2::GetPathId() const
{ return m_szPathID; }

IAsyncWriteRequest2::IAsyncWriteRequest2( const char *pFileName, const char *pPathID )
: m_fileName( pFileName ), m_szPathID( pPathID )
{}

IAsyncWriteRequest2::~IAsyncWriteRequest2()
{}

const char * IAsyncWriteRequest2::GetFileName() const
{ return m_fileName.Get(); }

const char * IAsyncWriteRequest2::GetPathId() const
{ return m_szPathID; }

IAsyncFileSystem2::IReadRequestBuilder::~IReadRequestBuilder()
{}

IAsyncFileSystem2::IReadRequestBuilder::This * IAsyncFileSystem2::IReadRequestBuilder::SetReadBytes( size_t nReadBytes )
{ m_nReadBytes = nReadBytes; return this; }

size_t IAsyncFileSystem2::IReadRequestBuilder::GetReadBytes() const
{ return m_nReadBytes; }

IAsyncFileSystem2::IReadRequestBuilder::This * IAsyncFileSystem2::IReadRequestBuilder::SetReadOffset( int64_t nReadOffset )
{ m_nReadOffset = nReadOffset; return this; }

int64_t IAsyncFileSystem2::IReadRequestBuilder::GetReadOffset() const
{ return m_nReadOffset; }

IAsyncFileSystem2::IReadRequestBuilder::This * IAsyncFileSystem2::IReadRequestBuilder::SetAllowMemoryMap( bool bAllowMemoryMap )
{ m_bAllowMemoryMap = bAllowMemoryMap; return this; }

bool IAsyncFileSystem2::IReadRequestBuilder::GetAllowMemoryMap() const
{ return m_bAllowMemoryMap; }

IAsyncFileSystem2::IReadRequestBuilder::This * IAsyncFileSystem2::IReadRequestBuilder::SetPreferMemoryMap( bool bPreferMemoryMap )
{ m_bPreferMemoryMap = bPreferMemoryMap; return this; }

bool IAsyncFileSystem2::IReadRequestBuilder::GetPreferMemoryMap() const
{ return m_bPreferMemoryMap; }

IAsyncFileSystem2::IReadRequestBuilder::This * IAsyncFileSystem2::IReadRequestBuilder::SetPriority( AsyncRequestPriority_t priority )
{ m_priority = priority; return this; }

AsyncRequestPriority_t IAsyncFileSystem2::IReadRequestBuilder::GetPriority() const
{ return m_priority; }

IAsyncFileSystem2::IReadRequestBuilder::This * IAsyncFileSystem2::IReadRequestBuilder::SetMoveToFrontOfQueue( bool bFrontOfQueue )
{ m_bFrontOfQueue = bFrontOfQueue; return this; }

bool IAsyncFileSystem2::IReadRequestBuilder::GetMoveToFrontOfQueue() const
{ return m_bFrontOfQueue; }

IAsyncFileSystem2::IReadRequestBuilder::This * IAsyncFileSystem2::IReadRequestBuilder::SetTextMode( bool bTextMode )
{ m_bTextMode = bTextMode; return this; }

bool IAsyncFileSystem2::IReadRequestBuilder::GetTextMode() const
{ return m_bTextMode; }

IAsyncFileSystem2::IReadRequestBuilder::This * IAsyncFileSystem2::IReadRequestBuilder::SetProfilingForRequest( bool bProfile )
{ m_bProfile = bProfile; return this; }

bool IAsyncFileSystem2::IReadRequestBuilder::GetProfilingForRequest() const
{ return m_bProfile; }

IAsyncFileSystem2::IReadRequestBuilder::This * IAsyncFileSystem2::IReadRequestBuilder::SetResultBuffer( void *pResultBuffer, size_t nResultBufferSize )
{ m_pResultBuffer = pResultBuffer; m_nResultBufferSize = nResultBufferSize; return this; }

void * IAsyncFileSystem2::IReadRequestBuilder::GetResultBuffer() const
{ return m_pResultBuffer; }

size_t IAsyncFileSystem2::IReadRequestBuilder::GetResultBufferSize() const
{ return m_nResultBufferSize; }

IAsyncFileSystem2::IReadRequestBuilder::This * IAsyncFileSystem2::IReadRequestBuilder::SetCompletionQueue( IIOCompletionQueue *pCompletionQueue )
{ m_pCompletionQueue = pCompletionQueue; return this; }

IIOCompletionQueue * IAsyncFileSystem2::IReadRequestBuilder::GetCompletionQueue() const
{ return m_pCompletionQueue; }

IAsyncFileSystem2::IReadRequestBuilder::This * IAsyncFileSystem2::IReadRequestBuilder::SetCallback( ReadCallback_t callback )
{ m_callback = std::move( callback ); return this; }

IAsyncFileSystem2::IReadRequestBuilder::IReadRequestBuilder()
: m_bProfile( false ), m_bAllowMemoryMap( false ), m_bPreferMemoryMap( false ), m_bTextMode( false ), m_bFrontOfQueue( false ),
	m_nReadOffset( -1 ), m_nReadBytes( 0 ), m_pResultBuffer( NULL ), m_nResultBufferSize( 0 ), m_priority( ASYNC_REQUEST_PRIORITY_DEFAULT ), m_pCompletionQueue( NULL )
{}

IAsyncFileSystem2::IWriteRequestBuilder::~IWriteRequestBuilder()
{}

IAsyncFileSystem2::IWriteRequestBuilder::This * IAsyncFileSystem2::IWriteRequestBuilder::SetPriority( AsyncRequestPriority_t priority )
{ m_priority = priority; return this; }

AsyncRequestPriority_t IAsyncFileSystem2::IWriteRequestBuilder::GetPriority() const
{ return m_priority; }

IAsyncFileSystem2::IWriteRequestBuilder::This * IAsyncFileSystem2::IWriteRequestBuilder::SetProfilingForRequest( bool bProfile )
{ m_bProfile = bProfile; return this; }

bool IAsyncFileSystem2::IWriteRequestBuilder::GetProfilingForRequest() const
{ return m_bProfile; }

IAsyncFileSystem2::IWriteRequestBuilder::This * IAsyncFileSystem2::IWriteRequestBuilder::SetTextMode( bool bTextMode )
{ m_bTextMode = bTextMode; return this; }

bool IAsyncFileSystem2::IWriteRequestBuilder::GetTextMode() const
{ return m_bTextMode; }

IAsyncFileSystem2::IWriteRequestBuilder::This * IAsyncFileSystem2::IWriteRequestBuilder::SetAppend( bool bAppend )
{ m_bAppend = bAppend; return this; }

bool IAsyncFileSystem2::IWriteRequestBuilder::GetAppend() const
{ return m_bAppend; }

IAsyncFileSystem2::IWriteRequestBuilder::This * IAsyncFileSystem2::IWriteRequestBuilder::SetFreeBufferCallback( std::function<void( const void *pBuffer )> freeBuffer )
{ m_freeBuffer = std::move( freeBuffer ); return this; }

IAsyncFileSystem2::IWriteRequestBuilder::This * IAsyncFileSystem2::IWriteRequestBuilder::SetCallback( WriteCallback_t callback )
{ m_callback = std::move( callback ); return this; }

IAsyncFileSystem2::IWriteRequestBuilder::IWriteRequestBuilder()
: m_bProfile( false ), m_bTextMode( false ), m_bAppend( false ), m_priority( ASYNC_REQUEST_PRIORITY_DEFAULT )
{}
