#include "public/const.h"

bool  IsSolid( SolidType_t solidType, int nSolidFlags )
{
	return (solidType != SOLID_NONE) && ((nSolidFlags & FSOLID_NOT_SOLID) == 0);
}
