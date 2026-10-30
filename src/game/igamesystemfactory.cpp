#include "public/game/igamesystemfactory.h"

CBaseGameSystemFactory** CBaseGameSystemFactory::sm_pFirst = nullptr;

void CBaseGameSystemFactory::SetFirstPtr(CBaseGameSystemFactory** ppFirst)
{
	sm_pFirst = ppFirst;
}

CBaseGameSystemFactory::CBaseGameSystemFactory(const char* pName)
{
	m_pName = pName;
	m_pNext = *CBaseGameSystemFactory::sm_pFirst;
	*CBaseGameSystemFactory::sm_pFirst = this;
}

IGameSystem* CBaseGameSystemFactory::GetGlobalPtrByName(const char* pName)
{
	CBaseGameSystemFactory* pFactoryList = *CBaseGameSystemFactory::sm_pFirst;
	while (pFactoryList)
	{
		if (strcmp(pFactoryList->m_pName, pName) == 0)
		{
			return pFactoryList->GetStaticGameSystem();
		}
		pFactoryList = pFactoryList->m_pNext;
	}
	return nullptr;
}
