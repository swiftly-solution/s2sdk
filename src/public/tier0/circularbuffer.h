#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

class CCircularBuffer
{
public:
	// Creates circular buffer that works as a FIFO, it would wrap the buffer once it reaches it's size
	// and overwrite past data.
	DLL_CLASS_IMPORT CCircularBuffer();
	// Creates circular buffer that works as a FIFO and allocates a buffer of a size nSize (would use 64 if nSize is less than that),
	// it would wrap the buffer once it reaches it's size and overwrite past data.
	DLL_CLASS_IMPORT CCircularBuffer( uint32 nSize );

	DLL_CLASS_IMPORT ~CCircularBuffer();

	// Resizes internal buffer with nSize size, using 0 as a size would free up the buffer
	DLL_CLASS_IMPORT void Resize( int nSize );

	// Peeks into a buffer and writes to pOut without consuming bytes,
	// number of bytes written is returned
	DLL_CLASS_IMPORT int Peek( void *pOut, int nCount );
	// Reads from the buffer to pOut and consumes bytes,
	// number of bytes written is returned
	DLL_CLASS_IMPORT int Read( void *pOut, int nCount );

	// Consumes nCount bytes from the buffer
	DLL_CLASS_IMPORT int Advance( int nCount );

	// Clears the internal buffer
	DLL_CLASS_IMPORT void Flush();

	// Writes from pData nCount bytes to buffer, leaving it ready for reading
	DLL_CLASS_IMPORT int Write( const void *pData, int nCount );
	// Writes zeroed nCount bytes of data to buffer, leaving it ready for reading
	DLL_CLASS_IMPORT int WriteZeroed( int nCount );

	int GetSize() const;
	int GetReadAvailable() const;
	int GetWriteAvailable() const;

protected:
	DLL_CLASS_IMPORT void AssertValid();

private:
	int m_nCount;

	int m_nRead;
	int m_nWrite;

	int m_nSize;

	void *m_pData;
};

#endif /* CIRCULARBUFFER_H */