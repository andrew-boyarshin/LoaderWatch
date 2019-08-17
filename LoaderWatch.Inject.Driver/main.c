#include "LoaderWatch.Inject.Driver.Library/injlib.h"

#include <ntddk.h>

_Dispatch_type_(IRP_MJ_CREATE)
_Dispatch_type_(IRP_MJ_CLOSE)
DRIVER_DISPATCH SioctlCreateClose;

_Dispatch_type_(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH SioctlDeviceControl;

UNICODE_STRING RegFilterAltitude = RTL_CONSTANT_STRING(L"372187");
LARGE_INTEGER RegFilterCookie = { 0 };

UNICODE_STRING RegFilterTargetKey = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\FileSystem\\");

union
{
    UINT32 Flags;

    struct {
        UINT32 InjLibInitialized : 1;
        UINT32 CreateProcessNotifyRoutineSet : 1;
        UINT32 LoadImageNotifyRoutineSet : 1;
        UINT32 CmCallbackRegistered : 1;
    };
}
InitializedFlags = { 0 };

VOID
PrintIrpInfo(
    PIRP Irp);

NTKERNELAPI
PCHAR
NTAPI
PsGetProcessImageFileName(
    _In_ PEPROCESS Process
);

//////////////////////////////////////////////////////////////////////////
// Helper functions.
//////////////////////////////////////////////////////////////////////////

//
// Taken from ReactOS, used by InjpInitializeDllPaths.
//

typedef union
{
    WCHAR Name[sizeof(ULARGE_INTEGER) / sizeof(WCHAR)];
    ULARGE_INTEGER Alignment;
} ALIGNEDNAME;

//
// DOS Device Prefix \??\
//

ALIGNEDNAME ObpDosDevicesShortNamePrefix = { { L'\\', L'?', L'?', L'\\' } };
UNICODE_STRING ObpDosDevicesShortName = {
  sizeof(ObpDosDevicesShortNamePrefix), // Length
  sizeof(ObpDosDevicesShortNamePrefix), // MaximumLength
  (PWSTR)& ObpDosDevicesShortNamePrefix  // Buffer
};

NTSTATUS
NTAPI
InjpJoinPath(
    _In_ PUNICODE_STRING Directory,
    _In_ PUNICODE_STRING Filename,
    _Inout_ PUNICODE_STRING FullPath
)
{
    UNICODE_STRING UnicodeBackslash = RTL_CONSTANT_STRING(L"\\");

    BOOLEAN DirectoryEndsWithBackslash = Directory->Length > 0 &&
        Directory->Buffer[Directory->Length - 1] == L'\\';

    if (FullPath->MaximumLength < Directory->Length ||
        FullPath->MaximumLength - Directory->Length -
        (!DirectoryEndsWithBackslash ? 1 : 0) < Filename->Length)
    {
        return STATUS_DATA_ERROR;
    }

    RtlCopyUnicodeString(FullPath, Directory);

    if (!DirectoryEndsWithBackslash)
    {
        RtlAppendUnicodeStringToString(FullPath, &UnicodeBackslash);
    }

    RtlAppendUnicodeStringToString(FullPath, Filename);

    return STATUS_SUCCESS;
}

NTSTATUS
NTAPI
InjCreateSettings(
    _In_ PUNICODE_STRING RegistryPath,
    _Inout_ PINJ_SETTINGS Settings
)
{
    UNREFERENCED_PARAMETER(Settings);

    // In the "ImagePath" key of the RegistryPath, there
    // is a full path of this driver file.  Fetch it.

    NTSTATUS Status;

    UNICODE_STRING ValueName = RTL_CONSTANT_STRING(L"ImagePath");

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes,
        RegistryPath,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL,
        NULL);

    HANDLE KeyHandle;
    Status = ZwOpenKey(&KeyHandle,
        KEY_READ,
        &ObjectAttributes);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    //
    // Save all information on stack - simply fail if path
    // is too long.
    //

    UCHAR KeyValueInformationBuffer[sizeof(KEY_VALUE_FULL_INFORMATION) + sizeof(WCHAR) * 128];
    PKEY_VALUE_FULL_INFORMATION KeyValueInformation = (PKEY_VALUE_FULL_INFORMATION)KeyValueInformationBuffer;

    ULONG ResultLength;
    Status = ZwQueryValueKey(KeyHandle,
        &ValueName,
        KeyValueFullInformation,
        KeyValueInformation,
        sizeof(KeyValueInformationBuffer),
        &ResultLength);

    ZwClose(KeyHandle);

    //
    // Check for succes.  Also check if the value is of expected
    // type and whether the path has a meaninful length.
    //

    if (!NT_SUCCESS(Status) ||
        KeyValueInformation->Type != REG_EXPAND_SZ ||
        KeyValueInformation->DataLength < sizeof(ObpDosDevicesShortNamePrefix))
    {
        return Status;
    }

    // todo: options?

    return STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
// DriverEntry and DriverDestroy.
//////////////////////////////////////////////////////////////////////////

DRIVER_UNLOAD DriverDestroy;

_Use_decl_annotations_
VOID
NTAPI
DriverDestroy(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    if (InitializedFlags.CmCallbackRegistered)
        CmUnRegisterCallback(RegFilterCookie);

    if (InitializedFlags.LoadImageNotifyRoutineSet)
        PsRemoveLoadImageNotifyRoutine(&InjLoadImageNotifyRoutine);

    if (InitializedFlags.CreateProcessNotifyRoutineSet)
        PsSetCreateProcessNotifyRoutineEx(&InjCreateProcessNotifyRoutineEx, TRUE);

    if (InitializedFlags.InjLibInitialized)
        InjDestroy();

    // Create counted string version of our Win32 device name.

    UNICODE_STRING uniWin32NameString;
    RtlInitUnicodeString(&uniWin32NameString, DOS_DEVICE_NAME);

    // Delete the link from our device name to a name in the Win32 namespace.

    IoDeleteSymbolicLink(&uniWin32NameString);

    if (DriverObject->DeviceObject)
    {
        IoDeleteDevice(DriverObject->DeviceObject);
    }
}

_Use_decl_annotations_
NTSTATUS
SioctlCreateClose(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    PAGED_CODE();

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

_Use_decl_annotations_
NTSTATUS
SioctlDeviceControl(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
)
/*++
Routine Description:
    This routine is called by the I/O system to perform a device I/O
    control function.
Arguments:
    DeviceObject - a pointer to the object that represents the device
        that I/O is to be done on.
    Irp - a pointer to the I/O Request Packet for this request.
Return Value:
    NT status code
--*/
{
    PIO_STACK_LOCATION  irpSp;// Pointer to current stack location
    NTSTATUS            ntStatus = STATUS_SUCCESS;// Assume success
    ULONG               inBufLength; // Input buffer length

    UNREFERENCED_PARAMETER(DeviceObject);

    PAGED_CODE();

    irpSp = IoGetCurrentIrpStackLocation(Irp);
    inBufLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;

    if (!inBufLength)
    {
        InjDbgPrint("[injdrv]: !inBufLength\n");
        ntStatus = STATUS_INVALID_PARAMETER;
        goto End;
    }

    // Determine which I/O control code was specified.

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
    {
    case IOCTL_LOADERWATCHDEBUG_INJECT:
        // In this method the I/O manager allocates a buffer large enough to
        // to accommodate larger of the user input buffer and output buffer,
        // assigns the address to Irp->AssociatedIrp.SystemBuffer, and
        // copies the content of the user input buffer into this SystemBuffer

        InjDbgPrint("[injdrv]: Called IOCTL_LOADERWATCHDEBUG_INJECT\n");
        PrintIrpInfo(Irp);

        if (inBufLength < sizeof(FIELD_OFFSET(LOADERWATCHDEBUG_INJECT_DATA, DllPath[5])))
        {
            InjDbgPrint("[injdrv]: IOCTL_LOADERWATCHDEBUG_INJECT received input too small (generic)\n");
            ntStatus = STATUS_INVALID_PARAMETER;
            break;
        }

        // Input buffer and output buffer is same in this case, read the
        // content of the buffer before writing to it
        PLOADERWATCHDEBUG_INJECT_DATA Data = Irp->AssociatedIrp.SystemBuffer;

        if (inBufLength < sizeof(FIELD_OFFSET(LOADERWATCHDEBUG_INJECT_DATA, DllPath[Data->DllLength])))
        {
            InjDbgPrint("[injdrv]: IOCTL_LOADERWATCHDEBUG_INJECT received input too small (for %lu)\n", Data->DllLength);
            ntStatus = STATUS_INVALID_PARAMETER;
            break;
        }

        // Read the data from the buffer

        PINJ_INJECTION_INFO Info = InjFindInjectionInfo(Data->ProcessId);

        if (!Info)
        {
            InjDbgPrint("[injdrv]: IOCTL_LOADERWATCHDEBUG_INJECT couldn't find injection info\n");
            ntStatus = STATUS_NOT_FOUND;
            break;
        }

        if (Info->IsInjected)
        {
            InjDbgPrint("[injdrv]: IOCTL_LOADERWATCHDEBUG_INJECT already injected\n");
            ntStatus = STATUS_ACCESS_DENIED;
            break;
        }

        SIZE_T DllPathBytes = sizeof(WCHAR) * Data->DllLength + sizeof(WCHAR);
        PWCHAR DllPathAlloc = ExAllocatePoolWithTag(NonPagedPoolNx,
            DllPathBytes,
            INJ_PATH_MEMORY_TAG);

        if (!DllPathAlloc)
        {
            InjDbgPrint("[injdrv]: IOCTL_LOADERWATCHDEBUG_INJECT unable to allocate %zu bytes for DLL path\n", DllPathBytes);
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        RtlZeroMemory(DllPathAlloc, DllPathBytes);
        RtlCopyMemory(DllPathAlloc, Data->DllPath, sizeof(WCHAR) * Data->DllLength);

        RtlInitUnicodeString(&Info->InjectionDllPath, DllPathAlloc);

        Info->SuppressInjectionOnModuleLoad = (BOOLEAN)Data->SuppressInjectionOnModuleLoad;
        Info->LogAllOpenKey = (BOOLEAN)Data->LogAllOpenKey;
        Info->PathAllocated = TRUE;

        InjDbgPrint("[injdrv]: Prepared for injection of (PID: %lu) with \"%wZ\"\n",
            (ULONG)(ULONG_PTR)Data->ProcessId,
            &Info->InjectionDllPath);

        // Assign the length of the data copied to IoStatus.Information
        // of the Irp and complete the Irp.

        Irp->IoStatus.Information = 0;

        // When the Irp is completed the content of the SystemBuffer
        // is copied to the User output buffer and the SystemBuffer is
        // is freed.

        break;
    default:
        // The specified I/O control code is unrecognized by this driver.

        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
        InjDbgPrint("[injdrv]: ERROR: unrecognized IOCTL %x\n",
            irpSp->Parameters.DeviceIoControl.IoControlCode);
        break;
    }

End:
    // Finish the I/O operation by simply completing the packet and returning
    // the same status as in the packet itself.

    Irp->IoStatus.Status = ntStatus;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return ntStatus;
}

VOID
PrintIrpInfo(
    PIRP Irp)
{
    PIO_STACK_LOCATION  irpSp;
    irpSp = IoGetCurrentIrpStackLocation(Irp);

    PAGED_CODE();

    InjDbgPrint("\tIrp->AssociatedIrp.SystemBuffer = 0x%p\n",
        Irp->AssociatedIrp.SystemBuffer);
    InjDbgPrint("\tIrp->UserBuffer = 0x%p\n", Irp->UserBuffer);
    InjDbgPrint("\tirpSp->Parameters.DeviceIoControl.Type3InputBuffer = 0x%p\n",
        irpSp->Parameters.DeviceIoControl.Type3InputBuffer);
    InjDbgPrint("\tirpSp->Parameters.DeviceIoControl.InputBufferLength = %lu\n",
        irpSp->Parameters.DeviceIoControl.InputBufferLength);
    InjDbgPrint("\tirpSp->Parameters.DeviceIoControl.OutputBufferLength = %lu\n",
        irpSp->Parameters.DeviceIoControl.OutputBufferLength);
    return;
}

EX_CALLBACK_FUNCTION InjRegistryCallback;

_Use_decl_annotations_
NTSTATUS
InjRegistryCallback(
    _In_ PVOID CallbackContext,
    _In_opt_ PVOID Argument1,
    _In_opt_ PVOID Argument2
)
{
    UNREFERENCED_PARAMETER(CallbackContext);

    REG_NOTIFY_CLASS NotifyClass = (REG_NOTIFY_CLASS)(ULONG_PTR)Argument1;

    if (!Argument2)
        return STATUS_SUCCESS;

    switch (NotifyClass)
    {
    case RegNtPreOpenKey:
    case RegNtPreOpenKeyEx:
        break;
    default:
        return STATUS_SUCCESS;
    }

    PINJ_INJECTION_INFO Info;

    Info = InjFindInjectionInfo(PsGetCurrentProcessId());

    if (!Info)
        return STATUS_SUCCESS;

    if (!Info->PathAllocated)
        return STATUS_SUCCESS;

    PUNICODE_STRING CompleteName;

    switch (NotifyClass)
    {
    case RegNtPreOpenKey:
        CompleteName = ((PREG_PRE_OPEN_KEY_INFORMATION)Argument2)->CompleteName;
        break;
    case RegNtPreOpenKeyEx:
        CompleteName = ((PREG_OPEN_KEY_INFORMATION)Argument2)->CompleteName;
        break;
    default:
        KeBugCheckEx(0x02000001, (ULONG_PTR)NotifyClass, (ULONG_PTR)Argument2, 0, 0);
    }

    if (Info->LogAllOpenKey)
    {
        InjDbgPrint(
            "[injdrv]: {%lu, \"%s\"} 0x%X(\"%wZ\")\n",
            PsGetCurrentProcessId(),
            PsGetProcessImageFileName(PsGetCurrentProcess()),
            NotifyClass,
            CompleteName
        );
    }

    if (RtlEqualUnicodeString(CompleteName, &RegFilterTargetKey, TRUE))
    {
        if (Info->IsInjected)
            return STATUS_SUCCESS;

        NTSTATUS Status;
        if (Info->IndirectInjection)
        {
            Status = InjpQueueApc(
                KernelMode,
                &InjpInjectApcNormalRoutine,
                Info,
                NULL,
                NULL
            );
        }
        else
        {
            Status = InjInject(Info);
        }

        InjDbgPrint("[injdrv]: Injection -> 0x%08lX\n", Status);

        Info->IsInjected = TRUE;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
DriverEntryImpl(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    NTSTATUS Status;
    UNICODE_STRING  ntUnicodeString;
    UNICODE_STRING  ntWin32NameString;
    PDEVICE_OBJECT  deviceObject = NULL;

    RtlInitUnicodeString(&ntUnicodeString, NT_DEVICE_NAME);

    Status = IoCreateDevice(
        DriverObject,                   // Our Driver Object
        0,                              // We don't use a device extension
        &ntUnicodeString,               // Device name "\Device\SIOCTL"
        FILE_DEVICE_UNKNOWN,            // Device type
        FILE_DEVICE_SECURE_OPEN,     // Device characteristics
        FALSE,                          // Not an exclusive device
        &deviceObject);                // Returned ptr to Device Object

    if (!NT_SUCCESS(Status))
    {
        InjDbgPrint("[injdrv]: Couldn't create the device object\n");
        return Status;
    }

    //
    // Initialize the driver object with this driver's entry points.
    //

    DriverObject->MajorFunction[IRP_MJ_CREATE] = SioctlCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = SioctlCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = SioctlDeviceControl;


    //
    // Initialize a Unicode String containing the Win32 name
    // for our device.
    //

    RtlInitUnicodeString(&ntWin32NameString, DOS_DEVICE_NAME);

    //
    // Create a symbolic link between our device name  and the Win32 name
    //

    Status = IoCreateSymbolicLink(
        &ntWin32NameString, &ntUnicodeString);

    if (!NT_SUCCESS(Status))
    {
        InjDbgPrint("[injdrv]: Couldn't create symbolic link\n");
        return Status;
    }


    //
    // Register DriverUnload routine.
    //

    DriverObject->DriverUnload = &DriverDestroy;

    //
    // Create injection settings.
    //

    INJ_SETTINGS Settings = { 0 };

    Status = InjCreateSettings(RegistryPath, &Settings);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

#if defined (_M_IX86)
    Settings.Method = InjMethodThunk;
#elif defined (_M_AMD64)
    Settings.Method = InjMethodThunkless;
#endif

    //
    // Initialize injection driver.
    //

    Status = InjInitialize(DriverObject, RegistryPath, &Settings);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    InitializedFlags.InjLibInitialized = TRUE;

    // Install CreateProcess and LoadImage notification routines.

    Status = PsSetCreateProcessNotifyRoutineEx(&InjCreateProcessNotifyRoutineEx, FALSE);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    InitializedFlags.CreateProcessNotifyRoutineSet = TRUE;

    Status = PsSetLoadImageNotifyRoutine(&InjLoadImageNotifyRoutine);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    InitializedFlags.LoadImageNotifyRoutineSet = TRUE;

    Status = CmRegisterCallbackEx(
        InjRegistryCallback,
        &RegFilterAltitude,
        DriverObject,
        NULL,
        &RegFilterCookie,
        NULL
    );

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    InitializedFlags.CmCallbackRegistered = TRUE;

    return STATUS_SUCCESS;
}

NTSTATUS
NTAPI
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    NTSTATUS Status;

    Status = DriverEntryImpl(DriverObject, RegistryPath);

    if (!NT_SUCCESS(Status))
    {
        DriverDestroy(DriverObject);
    }

    return Status;
}
