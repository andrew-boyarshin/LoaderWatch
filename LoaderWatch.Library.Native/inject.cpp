#include "common.h"

NTSYSAPI
NTSTATUS
NTAPI
RtlpCreateUserThreadEx(
    _In_ HANDLE Process,
    _In_opt_ PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
    _In_ ULONG Flags,
    _In_opt_ ULONG ZeroBits,
    _In_opt_ SIZE_T MaximumStackSize,
    _In_opt_ SIZE_T CommittedStackSize,
    _In_opt_ ULONG Unknown,
    _In_ PUSER_THREAD_START_ROUTINE StartAddress,
    _In_opt_ PVOID Parameter,
    _Out_opt_ PHANDLE Thread,
    _Out_opt_ PCLIENT_ID ClientId
);

decltype(RtlpCreateUserThreadEx)* RtlpCreateUserThreadExPtr = nullptr;

extern "C"
PHAPPAPI
void
NTAPI
InitCreateUserThreadEx(
    _In_ PVOID Pointer
)
{
    RtlpCreateUserThreadExPtr = static_cast<decltype(RtlpCreateUserThreadExPtr)>(Pointer);
}

NTSTATUS
NTAPI
CreateUserThreadEx(
    _In_ HANDLE Process,
    _In_opt_ PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
    _In_ ULONG Flags,
    _In_ PUSER_THREAD_START_ROUTINE StartAddress,
    _In_opt_ PVOID Parameter,
    _Out_opt_ PHANDLE Thread,
    _Out_opt_ PCLIENT_ID ClientId
)
{
    if (!RtlpCreateUserThreadExPtr)
    {
        PhShowError(NULL, (PWSTR)L"RtlpCreateUserThreadExPtr not initialized");
        return STATUS_UNSUCCESSFUL;
    }

    return RtlpCreateUserThreadExPtr(
        Process,
        ThreadSecurityDescriptor,
        Flags,
        0,
        0,
        0,
        0,
        StartAddress,
        Parameter,
        Thread,
        ClientId
    );
}

