#ifndef BYTESWAP_H
#define BYTESWAP_H

#if defined(_WIN32)
#pragma once
#endif

#include "platform.h"

class CByteswap
{
public:
	CByteswap();

	//-----------------------------------------------------------------------------
	// True if the current machine is detected as big endian. 
	// (Endienness is effectively detected at compile time when optimizations are
	// enabled)
	//-----------------------------------------------------------------------------
	static bool IsMachineBigEndian();

	//-----------------------------------------------------------------------------
	// Sets the target byte ordering we are swapping to or from.
	//
	// Braindead Endian Reference:
	//		x86 is LITTLE Endian
	//		PowerPC is BIG Endian
	//-----------------------------------------------------------------------------
	void SetTargetBigEndian( bool bigEndian );

	// Changes target endian
	void FlipTargetEndian( void );

	// Forces byte swapping state, regardless of endianess
	void ActivateByteSwapping( bool bActivate );

	//-----------------------------------------------------------------------------
	// Returns true if the target machine is the same as this one in endianness.
	//
	// Used to determine when a byteswap needs to take place.
	//-----------------------------------------------------------------------------
	bool IsSwappingBytes( void );

	bool IsTargetBigEndian( void );

	//-----------------------------------------------------------------------------
	// IsByteSwapped()
	//
	// When supplied with a chunk of input data and a constant or magic number
	// (in native format) determines the endienness of the current machine in
	// relation to the given input data.
	//
	// Returns:
	//		1  if input is the same as nativeConstant.
	//		0  if input is byteswapped relative to nativeConstant.
	//		-1 if input is not the same as nativeConstant and not byteswapped either.
	//
	// ( This is useful for detecting byteswapping in magic numbers in structure 
	// headers for example. )
	//-----------------------------------------------------------------------------
	template<typename T> inline int SourceIsNativeEndian( T input, T nativeConstant )
	{
		// If it's the same, it isn't byteswapped:
		if( input == nativeConstant )
			return 1;

		int output;
		LowLevelByteSwap<T>( &output, &input );
		if( output == nativeConstant )
			return 0;

		Assert( 0 );		// if we get here, input is neither a swapped nor unswapped version of nativeConstant.
		return -1;
	}

	//-----------------------------------------------------------------------------
	// Swaps an input buffer full of type T into the given output buffer.
	//
	// Swaps [count] items from the inputBuffer to the outputBuffer.
	// If inputBuffer is omitted or NULL, then it is assumed to be the same as
	// outputBuffer - effectively swapping the contents of the buffer in place.
	//-----------------------------------------------------------------------------
	template<typename T> inline void SwapBuffer( T* outputBuffer, T* inputBuffer = NULL, int count = 1 )
	{
		Assert( count >= 0 );
		Assert( outputBuffer );

		// Fail gracefully in release:
		if( count <=0 || !outputBuffer )
			return;

		// Optimization for the case when we are swapping in place.
		if( inputBuffer == NULL )
		{
			inputBuffer = outputBuffer;
		}

		// Swap everything in the buffer:
		for( int i = 0; i < count; i++ )
		{
			LowLevelByteSwap<T>( &outputBuffer[i], &inputBuffer[i] );
		}
	}

	//-----------------------------------------------------------------------------
	// Swaps an input buffer full of type T into the given output buffer.
	//
	// Swaps [count] items from the inputBuffer to the outputBuffer.
	// If inputBuffer is omitted or NULL, then it is assumed to be the same as
	// outputBuffer - effectively swapping the contents of the buffer in place.
	//-----------------------------------------------------------------------------
	template<typename T> inline void SwapBufferToTargetEndian( T* outputBuffer, T* inputBuffer = NULL, int count = 1 )
	{
		Assert( count >= 0 );
		Assert( outputBuffer );

		// Fail gracefully in release:
		if( count <=0 || !outputBuffer )
			return;

		// Optimization for the case when we are swapping in place.
		if( inputBuffer == NULL )
		{
			inputBuffer = outputBuffer;
		}

		// Are we already the correct endienness? ( or are we swapping 1 byte items? )
		if( !m_bSwapBytes || ( sizeof(T) == 1 ) )
		{
			// If we were just going to swap in place then return.
			if( !inputBuffer )
				return;
		
			// Otherwise copy the inputBuffer to the outputBuffer:
			if ( outputBuffer != inputBuffer )
				memcpy( outputBuffer, inputBuffer, count * sizeof( T ) );
			return;

		}

		// Swap everything in the buffer:
		for( int i = 0; i < count; i++ )
		{
			LowLevelByteSwap<T>( &outputBuffer[i], &inputBuffer[i] );
		}
	}

private:
	//-----------------------------------------------------------------------------
	// The lowest level byte swapping workhorse of doom.  output always contains the 
	// swapped version of input.  ( Doesn't compare machine to target endianness )
	//-----------------------------------------------------------------------------
	template<typename T> static void LowLevelByteSwap( T *output, T *input )
	{
		T temp = *output;
		for( size_t i = 0; i < sizeof(T); i++ )
		{
			((unsigned char* )&temp)[i] = ((unsigned char*)input)[sizeof(T)-(i+1)]; 
		}
		Q_memcpy( output, &temp, sizeof(T) );
	}

	unsigned int m_bSwapBytes : 1;
	unsigned int m_bBigEndian : 1;
};

#endif