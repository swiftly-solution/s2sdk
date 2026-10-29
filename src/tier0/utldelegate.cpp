#include "public/tier0/utldelegate.h"

bool CUtlAbstractDelegate:: IsLess(const CUtlAbstractDelegate &right) const
{
	// deal with static function pointers first
#if !defined(FASTDELEGATE_USESTATICFUNCTIONHACK)
	if (m_pStaticFunction !=0 || right.m_pStaticFunction!=0) 
		return m_pStaticFunction < right.m_pStaticFunction;
#endif
	if (m_pthis !=right.m_pthis) 
		return m_pthis < right.m_pthis;
	// There are no ordering operators for member function pointers, 
	// but we can fake one by comparing each byte. The resulting ordering is
	// arbitrary (and compiler-dependent), but it permits storage in ordered STL containers.
	return memcmp(&m_pFunction, &right.m_pFunction, sizeof(m_pFunction)) < 0;

}

bool CUtlAbstractDelegate::operator ! () const
{ 
	return m_pthis==0 && m_pFunction==0; 
}

bool CUtlAbstractDelegate:: IsEmpty() const
{ 
	return m_pthis==0 && m_pFunction==0; 
}

CUtlAbstractDelegate & CUtlAbstractDelegate::operator = (const CUtlAbstractDelegate &right)
{
	SetMementoFrom(right); 
	return *this;
}

bool CUtlAbstractDelegate::operator <(const CUtlAbstractDelegate &right)
{
	return IsLess(right);
}

bool CUtlAbstractDelegate::operator >(const CUtlAbstractDelegate &right)
{
	return right.IsLess(*this);
}

void CUtlAbstractDelegate::UnsafeThisPointerSlam( void *pThis )
{
m_pthis = (utldelegate::GenericClass*)( pThis );
}

void * CUtlAbstractDelegate::UnsafeGetThisPtr()
{
	return m_pthis;
}

void CUtlAbstractDelegate:: SetMementoFrom(const CUtlAbstractDelegate &right)
{
	m_pFunction = right.m_pFunction;
	m_pthis = right.m_pthis;
#if !defined(FASTDELEGATE_USESTATICFUNCTIONHACK)
	m_pStaticFunction = right.m_pStaticFunction;
#endif
}
