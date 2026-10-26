#ifndef FILENAMESYMBOLTABLE_H
#define FILENAMESYMBOLTABLE_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/stringpool.h"
#include "public/tier0/utlsymboltable.h"

typedef uint32_t FileNameHandle_t;

class CUtlFilenameSymbolTable
{
	struct FileNameHandleInternal_t
	{
		FileNameHandleInternal_t()
		{
			path = 0;
			file = 0;
		}

		unsigned short path;
		unsigned short file;
	};

public:
	FileNameHandle_t	FindOrAddFileName( const char *pFileName );
	FileNameHandle_t	FindFileName( const char *pFileName ) const;
	int					PathIndex(const FileNameHandle_t &handle) { return (( const FileNameHandleInternal_t * )&handle)->path; }
	bool				String( const FileNameHandle_t& handle, char *buf, int buflen );
	bool				String( const FileNameHandle_t& handle, CBufferString *pOut );
	void				RemoveAll();
	void				SpewStrings();
	bool				SaveToBuffer( CUtlBuffer &buffer );
	bool				RestoreFromBuffer( CUtlBuffer &buffer );

private:
	CCountedStringPool_CI	m_StringPool;
	mutable CThreadSpinRWLock m_lock;
};

#endif