#ifndef ISCENESYSTEM_H
#define ISCENESYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "public/const.h"
#include "public/tier0/bitvec.h"

struct vis_info_t
{
	uint32_t m_uVisBitsBufSize;
	SpawnGroupHandle_t m_SpawnGroupHandle;
	CBitVec<4096> m_VisBits;
};


#endif