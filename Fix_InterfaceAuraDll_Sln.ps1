<#
.SYNOPSIS
    在 UBT 重新生成 Aura.sln 之后，把 InterfaceAuraDll 工程补回解决方案。

.DESCRIPTION
    背景：Aura.sln 由 UnrealBuildTool 生成（也因此被 .gitignore 的 *.sln 屏蔽，
    改它不会进版本库）。UBT 每次重新生成工程文件都会整份重写 sln，
    InterfaceAuraDll 的工程项与它的配置映射会一起丢失，退回 VS 默认的 Win32
    映射 —— 于是编出 32 位 DLL，64 位的 UE 加载会失败（错误 193）。

    本脚本补齐工程项和 5 组解决方案配置的映射，全部指向 Release|x64。
    理由：UE 各配置（含 DebugGame）都用发布版 CRT —— 见 TargetRules 的
    bDebugBuildsActuallyUseDebugCRT，默认关闭。

    幂等：可以反复运行。

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File .\Fix_InterfaceAuraDll_Sln.ps1
#>

$ErrorActionPreference = 'Stop'

$SlnPath = Join-Path $PSScriptRoot 'Aura.sln'
$ProjectGuid = '{350CCEEB-5CEF-46C3-B3A5-426F673B35E7}'
$ProjectName = 'InterfaceAuraDll'
$ProjectRel = 'InterfaceAuraDll\InterfaceAuraDll.vcxproj'
$ProjectTypeGuid = '{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}'   # Visual C++

if (-not (Test-Path -LiteralPath $SlnPath)) {
    Write-Error "找不到 $SlnPath"
    exit 1
}

# 读入，并记住 BOM 与行尾，落盘时原样写回
$bytes = [System.IO.File]::ReadAllBytes($SlnPath)
$hasBom = ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF)
$text = [System.IO.File]::ReadAllText($SlnPath)
$newline = if ($text -match "`r`n") { "`r`n" } else { "`n" }

$lines = [System.Collections.Generic.List[string]]::new()
$lines.AddRange([string[]]($text -split "`r?`n"))

Write-Host "修补 $SlnPath"

# 1) 先清掉旧映射行，保证反复运行不会累积
#    注意：映射行在 .sln 里以两个 Tab 开头，必须 Trim 后再判断前缀
$before = $lines.Count
for ($i = $lines.Count - 1; $i -ge 0; $i--) {
    if ($lines[$i].Trim().StartsWith($ProjectGuid + '.')) { $lines.RemoveAt($i) }
}
if ($lines.Count -ne $before) {
    Write-Host ("  移除旧映射行 {0} 条" -f ($before - $lines.Count))
}

# 2) 补工程项
$projectLine = "Project(`"$ProjectTypeGuid`") = `"$ProjectName`", `"$ProjectRel`", `"$ProjectGuid`""
$hasProject = $false
foreach ($line in $lines) {
    if ($line -like "*$ProjectRel*") { $hasProject = $true; break }
}
if (-not $hasProject) {
    $globalIdx = $lines.IndexOf('Global')
    if ($globalIdx -lt 0) {
        Write-Error 'Aura.sln 结构异常：找不到 Global 行'
        exit 1
    }
    $lines.Insert($globalIdx, 'EndProject')
    $lines.Insert($globalIdx, $projectLine)
    Write-Host "  补回工程项 $ProjectName"
}

# 3) 补映射：全部 Release|x64
$solutions = @('DebugGame Editor', 'DebugGame', 'Development Editor', 'Development', 'Shipping')
$platforms = @('Win64', 'Win64-arm64', 'Win64-arm64ec')
$mapping = [System.Collections.Generic.List[string]]::new()
foreach ($config in $solutions) {
    foreach ($platform in $platforms) {
        $mapping.Add("`t`t$ProjectGuid.$config|$platform.ActiveCfg = Release|x64")
        # 只在 x64 解决方案平台下参与生成；arm64 / arm64ec 留 ActiveCfg 即可
        if ($platform -eq 'Win64') {
            $mapping.Add("`t`t$ProjectGuid.$config|$platform.Build.0 = Release|x64")
        }
    }
}

$startIdx = -1
for ($i = 0; $i -lt $lines.Count; $i++) {
    # 注意：.sln 里段落行带前导 Tab，必须 Trim 后再比
    if ($lines[$i].Trim() -like 'GlobalSection(ProjectConfigurationPlatforms)*') { $startIdx = $i; break }
}
if ($startIdx -lt 0) {
    Write-Error '找不到 GlobalSection(ProjectConfigurationPlatforms) 段'
    exit 1
}

$endIdx = -1
for ($i = $startIdx + 1; $i -lt $lines.Count; $i++) {
    if ($lines[$i].Trim() -eq 'EndGlobalSection') { $endIdx = $i; break }
}
if ($endIdx -lt 0) {
    Write-Error 'ProjectConfigurationPlatforms 段缺少结束标记'
    exit 1
}

$lines.InsertRange($endIdx, $mapping)
Write-Host ("  写入映射 {0} 条（全部 Release|x64）" -f $mapping.Count)

# 4) 落盘
$encoding = New-Object System.Text.UTF8Encoding($hasBom)
[System.IO.File]::WriteAllText($SlnPath, ($lines -join $newline), $encoding)
Write-Host '完成。'
