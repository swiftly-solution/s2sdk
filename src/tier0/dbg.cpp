#include "public/tier0/dbg.h"

void Error( const tchar* pMsg, ... )
{
	static char szBuffer[MAX_LOGGING_MESSAGE_LENGTH];
	va_list params;
	va_start(params, pMsg);
	V_vsnprintf(szBuffer, sizeof(szBuffer), pMsg, params);
	va_end(params);
	Plat_FatalError( "%s", szBuffer );
}

CDbgFmtMsg::CDbgFmtMsg(const tchar *pszFormat, ...)
{ 
	va_list arg_ptr;

	va_start(arg_ptr, pszFormat);
	Tier0Internal_vsntprintf(m_szBuf, sizeof(m_szBuf)-1, pszFormat, arg_ptr);
	va_end(arg_ptr);

	m_szBuf[sizeof(m_szBuf)-1] = 0;
}

CDbgFmtMsg::operator const tchar *() const
{ 
	return m_szBuf; 
}
