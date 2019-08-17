#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_M_AMD64) || defined(_M_ARM64)
# define INJ_CONFIG_SUPPORTS_WOW64
#endif

    // Device type
#define LOADERWATCHDEBUG_TYPE 45000

#define IOCTL_LOADERWATCHDEBUG_INJECT CTL_CODE(LOADERWATCHDEBUG_TYPE, 0x950, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define NT_DEVICE_NAME      L"\\Device\\LoaderWatchDebug"
#define DOS_DEVICE_NAME     L"\\DosDevices\\LoaderWatchDebug"

typedef struct _LOADERWATCHDEBUG_INJECT_DATA
{
    HANDLE ProcessId;
    union {
        UINT32 Flags;
        struct {
            UINT32 OverrideCanInject : 1;
            UINT32 SuppressInjectionOnModuleLoad : 1;
            UINT32 IndirectInjection : 1;
            UINT32 LogAllOpenKey : 1;
        };
    };
    ULONG DllLength;
    WCHAR DllPath[ANYSIZE_ARRAY];
} LOADERWATCHDEBUG_INJECT_DATA, * PLOADERWATCHDEBUG_INJECT_DATA;

#ifdef __cplusplus
}
#endif
