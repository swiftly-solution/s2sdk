#ifndef RAWALLOCATOR_H
#define RAWALLOCATOR_H

#ifdef _WIN32
#pragma once
#endif

#include <algorithm>
#include "memdbgon.h"

class CMemAllocAllocator
{
public:
	template<typename T, typename I = int>
	static T* Alloc( I nCount, I &nAdjustedCount )
	{
		size_t byte_size = nCount * sizeof( T );
		T *ptr = (T *)malloc( byte_size );
		nAdjustedCount = (I)(std::min)( (std::max)( _msize( ptr ), byte_size ) / sizeof( T ), (size_t)(std::numeric_limits<I>::max)() );
		return ptr;
	}

	template<typename T, typename I = int>
	static T* Realloc( T *base, I nCount, I &nAdjustedCount )
	{
		size_t byte_size = nCount * sizeof( T );
		T *ptr = (T *)realloc( base, byte_size );
		nAdjustedCount = (I)(std::min)( (std::max)( _msize( ptr ), byte_size ) / sizeof( T ), (size_t)(std::numeric_limits<I>::max)() );
		return ptr;
	}

	static void Free( void* pMem );
};

#include "memdbgoff.h"

#endif