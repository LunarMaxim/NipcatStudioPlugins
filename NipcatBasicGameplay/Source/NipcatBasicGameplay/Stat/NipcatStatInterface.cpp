// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatStatInterface.h"


// Add default functionality here for any INipcatStatInterface functions that are not pure virtual.
FNipcatStat& INipcatStatInterface::GetStat()
{
	K2_GetStat(CacheStat);
	return CacheStat;
}
