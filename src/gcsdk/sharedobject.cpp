#include "public/gcsdk/sharedobject.h"

namespace GCSDK {
CSharedObject::~CSharedObject()
{}
}

namespace GCSDK {
bool CSharedObject:: BIsKeyEqual( const CSharedObject &soRHS ) const
{ return !BIsKeyLess( soRHS ) && !soRHS.BIsKeyLess( *this ); }
}
