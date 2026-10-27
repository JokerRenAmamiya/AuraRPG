#include <windows.h>

// 注意：DllMain 运行在加载器锁(loader lock)之下，
// 不要在这里做内存分配、文件/控制台 IO、std::cout 等操作，否则可能死锁。
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}
