// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Aura : ModuleRules
{
	public Aura(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayTags",
			"GameplayTasks",
			"Niagara",
			"NavigationSystem"
		});

		// ==== InterfaceAuraDll（第三方 DLL，隐式链接）====
		// 边界规矩见 Docs/superpowers/specs/2026-10-12-auradll-ue-boundary-design.md
		string ThirdPartyPath = Path.Combine(ModuleDirectory, "ThirdParty");

		string PublicHeaderPath = Path.Combine(ThirdPartyPath, "AuraDllApi.h");
		string ImportLibPath = Path.Combine(ThirdPartyPath, "InterfaceAuraDll.lib");
		string RuntimeDllPath = Path.Combine(ThirdPartyPath, "InterfaceAuraDll.dll");

		// 产物缺失时给出可执行的报错，而不是让链接器抛出难以理解的符号缺失
		foreach (string RequiredFile in new string[] { PublicHeaderPath, ImportLibPath, RuntimeDllPath })
		{
			if (!File.Exists(RequiredFile))
			{
				throw new System.Exception(string.Format(
					"缺少 AuraDll 的构建产物: {0}\n" +
					"请先构建 InterfaceAuraDll 工程（Release|x64）：它的后构建事件会把 " +
					"AuraDllApi.h / InterfaceAuraDll.dll / InterfaceAuraDll.lib 同步到 Source/Aura/ThirdParty/。",
					RequiredFile));
			}
		}

		// 对外头位置：这样 "#include AuraDllApi.h" 才可用
		PublicIncludePaths.Add(ThirdPartyPath);

		// 导入库。DLL 本体由 RuntimeDependencies 放到可执行文件旁边
		PublicAdditionalLibraries.Add(ImportLibPath);

		// 把运行时 DLL 放到可执行文件旁边（编辑器即 Binaries/Win64）。
		// 注意：模块级的 RuntimeDependencies 用 $(TargetOutputDir) 不会真的拷贝，
		// 必须用 $(BinaryOutputDir)；已实测。
		RuntimeDependencies.Add("$(BinaryOutputDir)/InterfaceAuraDll.dll", RuntimeDllPath);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}