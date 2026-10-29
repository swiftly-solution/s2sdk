#include "public/filesystem/filesystem.h"

FileAsyncRequest_t::FileAsyncRequest_t()
{ memset( this, 0, sizeof(*this) ); hSpecificAsyncFile = FS_INVALID_ASYNC_FILE;	}

IMemoryMappedFile::IMemoryMappedFile()
: m_pMappedMemory( NULL ), m_nMappedSize( 0 ), m_nFlags( (MemoryMappedFileFlags_t)0 ), m_nRefCount( 1 )
{}

IMemoryMappedFile::~IMemoryMappedFile()
{}

uint32_t IMemoryMappedFile::AddRef()
{ return ++m_nRefCount; }

uint32_t IMemoryMappedFile::Release()
{
	uint32_t nRefCount = --m_nRefCount;
	if ( nRefCount == 0 )
		ReachedZeroReferences();
	return nRefCount;
}

const void * IMemoryMappedFile::GetMappedMemory() const
{ return m_pMappedMemory; }

size_t IMemoryMappedFile::GetMappedSize() const
{ return m_nMappedSize; }

MemoryMappedFileFlags_t IMemoryMappedFile::GetFlags() const
{ return m_nFlags; }
