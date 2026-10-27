#include "AuraDll/AuraDllFunctions.h"

namespace AuraDll
{
	bool CheckApiVersion()
	{
		return Aura_GetApiVersion() == AURA_API_VERSION;
	}

	int32 GetVersion()
	{
		return static_cast<int32>(Aura_GetVersion());
	}

	int32 Add(int32 A, int32 B)
	{
		return static_cast<int32>(Aura_Add(A, B));
	}
}
