#include "public/steamworks/steamhttpenums.h"

bool  BIsHTTPStatusSuccess( EHTTPStatusCode eStatus )
{
	return eStatus >= 200 && eStatus <= 299;
}
