#include "public/steamworks/steam_api_common.h"

CCallbackBase::CCallbackBase()
{ m_nCallbackFlags = 0; m_iCallback = 0; }

int CCallbackBase::GetICallback()
{ return m_iCallback; }
