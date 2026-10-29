#include "public/tier0/keyvaluessystem.h"

HKeySymbol::HKeySymbol()
: nIndex(0xFFFFFF)
{ }

HKeySymbol::HKeySymbol(uint32_t idx)
: nIndex(idx)
{ }

uint32_t HKeySymbol::Get()
{ return nIndex; }
