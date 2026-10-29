#include "public/tier0/refcount.h"

IRefCounted::~IRefCounted()
{}

int CRefMT:: Increment( int *p)
{ return ThreadInterlockedIncrement( (int32 *)p ); }

int CRefMT:: Decrement( int *p)
{ return ThreadInterlockedDecrement( (int32 *)p ); }

int CRefST:: Increment( int *p)
{ return ++(*p); }

int CRefST:: Decrement( int *p)
{ return --(*p); }

int CRefCountServiceNull:: DoAddRef()
{ return 1; }

int CRefCountServiceNull:: DoRelease()
{ return 1; }
