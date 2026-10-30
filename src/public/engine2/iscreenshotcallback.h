#ifndef ISCREENSHOTCALLBACK_H
#define ISCREENSHOTCALLBACK_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"

class IScreenshotCallback
{
public:
	virtual bool Screenshot( int nWidth, int nHeight, int nImageFormat, uint8_t *pPixels, int nSize, int64 nContext ) = 0;
	virtual bool ScreenshotJPEG( int nWidth, int nHeight, void *pData, int nSize, void *pContext ) = 0;
	virtual bool ScreenshotPNG( int nWidth, int nHeight, void *pData, int nSize, void *pContext ) = 0;
	virtual bool Screenshot_GPUFrame( uint32_t nWidth, uint32_t nHeight, int nImageFormat, const uint8_t *pPixels, size_t nPitch, int64 nContext, uint32_t ) = 0;
	virtual bool IsDoneWithData( void *pContext ) = 0;
};

#endif