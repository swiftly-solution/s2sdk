#include "public/filesystem/filenamesymboltable.h"

CUtlFilenameSymbolTable::FileNameHandleInternal_t::FileNameHandleInternal_t()
{
	path = 0;
	file = 0;
}

int CUtlFilenameSymbolTable::PathIndex(const FileNameHandle_t &handle)
{ return (( const FileNameHandleInternal_t * )&handle)->path; }
