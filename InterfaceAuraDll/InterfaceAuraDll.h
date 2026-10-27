// InterfaceAuraDll.h
// Aura 使用的第三方动态库接口
// >>> 下面只是示例接口，替换成你自己的函数即可 <<<
#pragma once

#ifdef INTERFACE_AURA_DLL_EXPORTS
#define INTERFACE_AURA_API __declspec(dllexport)
#else
#define INTERFACE_AURA_API __declspec(dllimport)
#endif

extern "C"
{
	// 返回 DLL 版本号（示例）
	INTERFACE_AURA_API int InterfaceAura_GetVersion();

	// 两数相加（示例）
	INTERFACE_AURA_API int InterfaceAura_Add(int A, int B);
}
