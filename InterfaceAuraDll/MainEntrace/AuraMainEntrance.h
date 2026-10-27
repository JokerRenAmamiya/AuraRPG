#pragma once

#include "Singleton.h"

// DLL 内部实现类：不导出。外部只通过 AuraDllApi.h 的 C 接口访问。
class AuraMainEntrance
{
public:
	friend class Singleton<AuraMainEntrance>; // 实现单例


	bool Initialize();
};
