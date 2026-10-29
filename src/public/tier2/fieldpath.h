#ifndef FIELDPATH_H
#define FIELDPATH_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform_base.h"

#define DEFAULT_MAX_PATH_DEPTH 10

class CFieldPath
{
public:
	union
	{
		// Used when m_bUsingExternalPaths is set
		int16_t *m_pPaths;
		int16_t m_FixedPaths[12];
	};

	int16_t m_nCount;
	bool m_bUsingExternalPaths;
};

#endif /* FIELDPATH_H */