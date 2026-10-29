#ifndef IWORLDRENDERERMGR_H
#define IWORLDRENDERERMGR_H

#ifdef _WIN32
#pragma once
#endif

#include "public/mathlib/mathlib.h"
#include "public/const.h"

class IWorld;

class IComputeWorldOriginCallback
{
public:
	virtual matrix3x4_t ComputeWorldOrigin( const char *pWorldName, SpawnGroupHandle_t hSpawnGroup, IWorld * pWorld ) = 0;
};

#endif