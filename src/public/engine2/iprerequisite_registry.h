#ifndef IPREREQUISITE_REGISTRY_H
#define IPREREQUISITE_REGISTRY_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"

class IPrerequisite;

abstract_class IPrerequisiteRegistry
{
public:
	virtual void RegisterPrerequisite( IPrerequisite * ) = 0;
};

#endif // IPREREQUISITE_REGISTRY_H
