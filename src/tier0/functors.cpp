#include "public/tier0/functors.h"

CFunctor::CFunctor()
{
#ifdef DEBUG
	m_nUserID = 0;
#endif
}

void CFuncMemPolicyNone::OnAcquire(void *pObject)
{}

void CFuncMemPolicyNone::OnRelease(void *pObject)
{}

void CFunctorCallbackBase::ValidateFunctorData( CFunctorData *pData )
{
#ifdef _DEBUG
	char pDataString[1024];
	char pCallbackString[1024];
	ComputeValidationString( pCallbackString, sizeof(pCallbackString) );
	pData->ComputeValidationString( pDataString, sizeof(pDataString) );
	bool bMatch = !Q_stricmp( pDataString, pCallbackString );
	if ( !bMatch )
	{
		Warning( "Functor doesn't match data!\n\tExpected:\t%s\n\tEncountered:\t%s\n",
			pCallbackString, pDataString );
		Assert( 0 );
	}
#endif
}

CFunctorData0::CFunctorData0( )
{}

void CFunctorData0::ComputeValidationString( char *pString, size_t nBufLen ) const
{ FUNC_VALIDATION_STRING_0 }

CFunctorCallback0::CFunctorCallback0( Callback_t pfnProxied )
: m_pfnProxied( pfnProxied )
{}

void CFunctorCallback0::operator()( CFunctorData *pFunctorDataBase )
{
	ValidateFunctorData( pFunctorDataBase );
	m_pfnProxied( );
}

void CFunctorCallback0::ComputeValidationString( char *pString, size_t nBufLen ) const
{ FUNC_VALIDATION_STRING_0 }

bool CFunctorCallback0::IsEqual( CFunctorCallback *pSrc ) const
{ 
	if ( Q_stricmp( GetImplClassName(), pSrc->GetImplClassName() ) )
		return false;
	return m_pfnProxied == static_cast< CFunctorCallback0* >( pSrc )->m_pfnProxied; 
}

const char * CFunctorCallback0::GetImplClassName() const
{ return "CFunctorCallback0"; }

const void * CFunctorCallback0::GetTarget() const
{ return reinterpret_cast<void *>(m_pfnProxied); }

CFunctorData * CreateFunctorData()
{
	return new CFunctorData0(); 
}

CFunctorCallback * CreateFunctorCallback( void (*pfnProxied)() )
{
	return new CFunctorCallback0( pfnProxied );
}
