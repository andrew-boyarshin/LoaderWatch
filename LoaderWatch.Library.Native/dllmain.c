// dllmain.cpp : Defines the entry point for the DLL application.
#include "common.h"
#include "mapimg.h"
#include "LoaderWatch.Inject.Driver.Library/injdata.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

PHAPPAPI
NTSTATUS
NTAPI
InitializePhLib()
{
    return PhInitializePhLibEx(
        L"LoaderWatch",
        ULONG_MAX, // all possible features
        NtCurrentPeb()->ImageBaseAddress,
        0,
        0
    );
}

PHAPPAPI
NTSTATUS
NTAPI
LoadDllProcess(
    _In_ HANDLE ProcessId,
    _In_ HANDLE ThreadId,
    _In_ PWSTR FileName
)
{
    HANDLE hDevice;
    NTSTATUS Status;

    hDevice = CreateFileW(L"\\\\.\\LoaderWatchDebug",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hDevice == INVALID_HANDLE_VALUE) {
        return STATUS_DRIVER_BLOCKED;
    }

    ULONG FileNameLength = (ULONG)wcslen(FileName);
    SIZE_T InjectDataLength = FIELD_OFFSET(LOADERWATCHDEBUG_INJECT_DATA, DllPath[FileNameLength + 1]);

    PLOADERWATCHDEBUG_INJECT_DATA InjectData = RtlAllocateHeap(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        InjectDataLength
    );

    RtlCopyMemory(InjectData->DllPath, FileName, FileNameLength * sizeof(WCHAR));

    InjectData->DllLength = FileNameLength;
    InjectData->ProcessId = ProcessId;
    InjectData->SuppressInjectionOnModuleLoad = TRUE;
    InjectData->IndirectInjection = TRUE;

#if 0
    IO_STATUS_BLOCK Block = { 0 };

    Status = NtDeviceIoControlFile(
        hDevice,
        NULL,
        NULL,
        NULL,
        &Block,
        IOCTL_LOADERWATCHDEBUG_INJECT,
        InjectData,
        (ULONG)InjectDataLength,
        NULL,
        0
    );

    if (Status == STATUS_PENDING)
    {
        Status = NtWaitForSingleObject(hDevice, FALSE, NULL);

        if (NT_SUCCESS(Status))
        {
            Status = Block.Status;
        }
    }
#else
    DWORD bytesReturned = 0;

    BOOL ret = DeviceIoControl(
        hDevice,
        IOCTL_LOADERWATCHDEBUG_INJECT,
        InjectData,
        (DWORD)InjectDataLength,
        NULL,
        0,
        &bytesReturned,
        NULL
    );

    Status = ret ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
#endif

    CloseHandle(hDevice);
    RtlFreeHeap(GetProcessHeap(), 0, InjectData);

    return Status;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved)
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
