#include "public/vscript/ivscript.h"

ScriptFuncDescriptor_t::ScriptFuncDescriptor_t()
{
	m_pszFunction = NULL;
	m_ReturnType = FIELD_TYPEUNKNOWN;
	m_iVariantCount = 0;
	m_iParamCount = 0;
	memset(m_Parameters, 0, sizeof(m_Parameters));
	m_pszDescription = NULL;
	m_pszParameterNames = NULL;
}

void * IScriptInstanceHelper::GetProxied( void *p )
{ return p; }

bool IScriptInstanceHelper::ToString( void *p, char *pBuf, int bufSize )
{ return false; }

void * IScriptInstanceHelper::BindOnRead( HSCRIPT hInstance, void *pOld, const char *pszId )
{ return NULL; }

ScriptClassDesc_t::ScriptClassDesc_t()
: m_pszScriptName( 0 ), m_pszClassname( 0 ), m_pszDescription( 0 ), m_pBaseDesc( 0 ), m_pfnConstruct( 0 ), m_pfnDestruct( 0 ), pHelper(NULL)
{}

IScriptVM::~IScriptVM()
{}

ScriptStatus_t IScriptVM::Run( const unsigned char *pszScript, bool bWait )
{ return Run( (char *)pszScript, bWait ); }

HSCRIPT IScriptVM::CompileScript( const unsigned char *pszScript, const char *pszId )
{ return CompileScript( (char *)pszScript, pszId ); }

void IScriptVM::RemoveInstance( HSCRIPT hInstance, const char *pszInstance, HSCRIPT hScope )
{ ClearValue( hScope, pszInstance ); RemoveInstance( hInstance ); }

void IScriptVM::RemoveInstance( const char *pszInstance, HSCRIPT hScope )
{ ScriptVariant_t val; if ( GetValue( hScope, pszInstance, &val ) ) { if ( val.GetType() == FIELD_HSCRIPT ) { RemoveInstance( val, pszInstance, hScope ); } ReleaseValue( val ); } }

bool IScriptVM::ValueExists( const char *pszKey )
{ return ValueExists( NULL, pszKey ); }

bool IScriptVM::SetValue( const char *pszKey, const ScriptVariant_t &value )
{ return SetValue(NULL, pszKey, value ); }

bool IScriptVM::GetValue( const char *pszKey, ScriptVariant_t *pValue )
{ return GetValue(NULL, pszKey, pValue ); }

bool IScriptVM::ClearValue( const char *pszKey)
{ return ClearValue( NULL, pszKey ); }
