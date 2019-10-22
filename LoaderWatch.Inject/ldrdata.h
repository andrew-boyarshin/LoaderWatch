#pragma once

typedef enum _LDR_HOOK_TARGETS
{
    HookTargetLdrpAllocateModuleEntry = 0,
    HookTargetLdrpMergeNodes,
    HookTargetLdrpDestroyNode,
    HookTargetLdrpMapDllWithSectionHandle,
    HookTargetLdrpFreeLoadContext,
    HookTargetLdrpUnmapModule,
    HookTargetLdrpCondenseGraphRecurse,
    HookTargetLdrpPreprocessDllName,
    HookTargetLdrpApplyFileNameRedirection,
    HookTargetsCount
} LDR_HOOK_TARGETS;
