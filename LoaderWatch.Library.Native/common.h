#pragma once

#if !defined(_PHAPP_)
#define PHAPPAPI __declspec(dllimport)
#else
#define PHAPPAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#define _PH_PHNATINL_H
#endif

#include <ph.h>

#ifndef __cplusplus
#include <provider.h>
#include <fastlock.h>
#include <phnet.h>
#include <symprv.h>
#endif

#include <dbghelp.h>


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
);

#ifdef __cplusplus
}
#endif
