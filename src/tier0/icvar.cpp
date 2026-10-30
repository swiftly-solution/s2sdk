#include "public/tier0/icvar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "public/tier0/memdbgon.h"

ICvar *g_pCVar = nullptr;

void SetGlobalCvarObject(ICvar* cvar)
{
    g_pCVar = cvar;
}

void IConVarListener:: OnConVarCreated( ConVarRefAbstract *pNewCvar )
{}

void IConVarListener:: OnConCommandCreated( ConCommand *pNewCommand )
{}

int ICvar::						GetMaxSplitScreenSlots() const
{ return m_MaxSplitScreenSlots; }

void * CCvar::AllocateMemory( int size )
{
	CAutoLock lock( m_Mutex );

	int aligned_size = ALIGN_VALUE( size, 8 );
	
	if(aligned_size + m_CurrentMemoryBufferSize > kMemoryBufferChunkMaxSize)
	{
		m_CurrentMemoryBufferSize = 0;
		*m_MemoryBuffer.AddToTailGetPtr() = (uint8_t *)malloc( kMemoryBufferChunkMaxSize );
	}

	int offs = m_CurrentMemoryBufferSize;
	m_CurrentMemoryBufferSize += aligned_size;

	return &m_MemoryBuffer[m_MemoryBuffer.Count() - 1][offs];
}

const char * CCvar::AllocateString( const char *string )
{
	CAutoLock lock( m_Mutex );

	if(!string || !string[0])
		return "";

	int strlen = V_strlen( string ) + 1;

	if(strlen + m_CurrentStringsBufferSize > kStringBufferChunkMaxSize)
	{
		m_CurrentStringsBufferSize = 0;
		*m_StringsBuffer.AddToTailGetPtr() = (char *)malloc( kStringBufferChunkMaxSize );
	}

	int offs = m_CurrentStringsBufferSize;
	m_CurrentStringsBufferSize += strlen;

	char *base = &m_StringsBuffer[m_StringsBuffer.Count() - 1][offs];
	V_memmove( base, string, strlen );

	return base;
}
