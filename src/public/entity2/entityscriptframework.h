#ifndef ENTITYSCRIPTFRAMEWORK_H
#define ENTITYSCRIPTFRAMEWORK_H

#ifdef _WIN32
#pragma once
#endif

#include "entitycomponent.h"
#include "public/tier0/utlsymbollarge.h"

class CScriptComponent : public CEntityComponent
{
public:
	virtual const char* GetComponentName() = 0;

public:
	HSCRIPT m_hScope;

	// AMNOTE: The storage of a CUtlVectorUltraConservative<HSCRIPT*>, an int count followed by the elements,
	// but null rather than CUtlVectorUltraConservative's static block when empty
	void* m_FuncHandles;

	int m_flags;
	CEntityInstance* m_pOwner;

	// AMNOTE: The storage of a CUtlVectorUltraConservative<CEntityIOOutput>, null when empty like m_FuncHandles
	void* m_Outputs;

	CUtlSymbolLarge m_scriptClassName;
};

#endif