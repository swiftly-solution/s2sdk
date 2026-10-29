#include "public/entity2/entityclass.h"

CSchemaClassInfo * CEntityClass::GetSchemaBinding() const
{
	return m_pClassInfo->m_pSchemaBinding;
}

datamap_t * CEntityClass::GetDataDescMap() const
{
	return m_pClassInfo->m_pDataDescMap;
}
