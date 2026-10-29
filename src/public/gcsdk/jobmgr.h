#ifndef JOBMGR_H
#define JOBMGR_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/threadtools.h"

namespace GCSDK
{

// AMNOTE: Opaque, only its size matches the game's. Its work thread pool embeds a CThreadEvent, which
// is 112 bytes larger on Linux (pthread mutex and condition) than on Windows (a HANDLE)
class CJobMgr
{
protected:
	uint8 m_unk001[784];
	CThreadEvent m_unk002;
};

} // namespace GCSDK

#endif // JOBMGR_H
