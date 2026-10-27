// AuraDll 无状态接口的 UE 侧包装。有状态的部分走 UAuraDllSubsystem。
// 边界规矩见 Docs/superpowers/specs/2026-10-12-auradll-ue-boundary-design.md
#pragma once

#include "CoreMinimal.h"
#include "AuraDllApi.h"

namespace AuraDll
{
	/** 头文件与运行时 DLL 的边界版本是否一致。 */
	bool CheckApiVersion();

	/** DLL 实现版本号。 */
	int32 GetVersion();

	/** 无状态示例：两数相加。 */
	int32 Add(int32 A, int32 B);
}
