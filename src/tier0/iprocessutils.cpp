#include "public/tier0/iprocessutils.h"

IProcess::~IProcess()
{}

int CEnvironmentBlock:: GetVarCount() const
{ return m_Vars.Count() - 1; }

char *const * CEnvironmentBlock::GetVars() const
{ return m_Vars.Base(); }

const char * CEnvironmentBlock::GetBlock() const
{ return m_Block.Base(); }
