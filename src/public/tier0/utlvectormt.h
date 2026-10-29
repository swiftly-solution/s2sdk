#ifndef UTLVECTORMT_H
#define UTLVECTORMT_H

#ifdef _WIN32
#pragma once
#endif

#include "threadtools.h"

//-----------------------------------------------------------------------------
// The CUtlVectorMT class:
// A array class with some sort of mutex protection. Not sure which operations are protected from
// which others.
//-----------------------------------------------------------------------------

template< class BASE_UTLVECTOR, class MUTEX_TYPE = CThreadFastMutex >
class CUtlVectorMT : public BASE_UTLVECTOR, public MUTEX_TYPE
{
	typedef BASE_UTLVECTOR BaseClass;
public:
	MUTEX_TYPE Mutex_t;

	using BaseClass::BaseClass;
};

#endif // UTLVECTORMT_H
