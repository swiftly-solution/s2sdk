#ifndef TIMEDEVENTMGR_H
#define TIMEDEVENTMGR_H

#if _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"

abstract_class IEventRegisterCallback
{
public:
	virtual void FireEvent() = 0;
};

#endif // TIMEDEVENTMGR_H
