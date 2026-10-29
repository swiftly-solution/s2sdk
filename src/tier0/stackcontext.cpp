#include "public/tier0/stackcontext.h"

bool MemoryRange_t:: Contains( const void *p ) const
{ return p >= pLow && p < pHigh; }

size_t MemoryRange_t:: Size() const
{ return (size_t)( (const uint8 *)pHigh - (const uint8 *)pLow ); }

JmpBuf_AlternateStack_t * StackContext_LongJumpLink_t::GetEnclosingJmpBuf() const
{ return (JmpBuf_AlternateStack_t *)this; }
