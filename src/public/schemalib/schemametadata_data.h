#ifndef SCHEMAMETADATA_DATA_H
#define SCHEMAMETADATA_DATA_H

#ifdef _WIN32
#pragma once
#endif

struct SchemaMetadataEntryData_t
{
	const char* m_pName;
	void* m_pData;
};

#endif