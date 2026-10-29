#include "public/tier1/tier1.h"

void  ConnectTier1Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount )
{ ConnectInterfaces( pFactoryList, nFactoryCount ); }

void  DisconnectTier1Libraries()
{ DisconnectInterfaces(); }
