#include "InterfaceAuraDll.h"

extern "C"
{
	int InterfaceAura_GetVersion()
	{
		return 1;
	}

	int InterfaceAura_Add(int A, int B)
	{
		return A + B;
	}
}
