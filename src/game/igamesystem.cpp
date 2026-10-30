#include "public/game/igamesystem.h"

IGameSystem::~IGameSystem()
{}

CAutoGameSystem::CAutoGameSystem(const char* pszInitName)
:  m_pName(pszInitName)
{
}

bool CAutoGameSystem::Init()
{ return true; }

void CAutoGameSystem::PostInit()
{}

void CAutoGameSystem::Shutdown()
{}

void CAutoGameSystem::unk_101( const void *const msg )
{}

void CAutoGameSystem::unk_201( const void *const msg )
{}

void CAutoGameSystem::unk_202( const void *const msg )
{}

const char* CAutoGameSystem::GetName() const
{ return m_pName; }

void CAutoGameSystem::SetGameSystemGlobalPtrs(void* pValue)
{}

void CAutoGameSystem::SetName(const char* pName)
{ m_pName = pName; }

bool CAutoGameSystem::DoesGameSystemReallocate()
{ return false; }

CAutoGameSystem::~CAutoGameSystem()
{}
