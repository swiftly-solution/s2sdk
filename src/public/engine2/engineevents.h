#ifndef ENGINEEVENTS_H
#define ENGINEEVENTS_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platwindow.h"
#include "public/inputsystem/inputenums.h"
#include "public/rendersystem/rendersystemtypes.h"

struct EngineLoopState_t
{
	PlatWindow_t m_hWnd;
	SwapChainHandle_t m_hSwapChain;
	InputContextHandle_t m_hInputContext;
	int m_nPlatWindowWidth;
	int m_nPlatWindowHeight;
	int m_nRenderWidth;
	int m_nRenderHeight;
};

struct EventClientOutput_t
{
	EngineLoopState_t m_LoopState;
	float m_flRenderTime;
	float m_flRealTime;
	float m_flRenderFrameTimeUnbounded;
	bool m_bRenderOnly;
};

#endif // ENGINEEVENTS_H
