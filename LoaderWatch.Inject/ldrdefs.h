#pragma once

//
// Dependencies
//
//
//
// Resource Type Levels
//
#define RESOURCE_TYPE_LEVEL                     0
#define RESOURCE_NAME_LEVEL                     1
#define RESOURCE_LANGUAGE_LEVEL                 2
#define RESOURCE_DATA_LEVEL                     3

//
// Dll Characteristics for LdrLoadDll
//
#define LDR_IGNORE_CODE_AUTHZ_LEVEL                 0x00001000

//
// LdrAddRef Flags
//
#define LDR_ADDREF_DLL_PIN                          0x00000001

//
// LdrLockLoaderLock Flags
//
#define LDR_LOCK_LOADER_LOCK_FLAG_RAISE_ON_ERRORS   0x00000001
#define LDR_LOCK_LOADER_LOCK_FLAG_TRY_ONLY          0x00000002

//
// LdrUnlockLoaderLock Flags
//
#define LDR_UNLOCK_LOADER_LOCK_FLAG_RAISE_ON_ERRORS 0x00000001

//
// LdrGetDllHandleEx Flags
//
#define LDR_GET_DLL_HANDLE_EX_UNCHANGED_REFCOUNT    0x00000001
#define LDR_GET_DLL_HANDLE_EX_PIN                   0x00000002


#define LDR_LOCK_LOADER_LOCK_DISPOSITION_INVALID           0
#define LDR_LOCK_LOADER_LOCK_DISPOSITION_LOCK_ACQUIRED     1
#define LDR_LOCK_LOADER_LOCK_DISPOSITION_LOCK_NOT_ACQUIRED 2

//
// FIXME: THIS SHOULD *NOT* BE USED!
//
#define IMAGE_SCN_TYPE_NOLOAD                   0x00000002

//
// Loader datafile/imagemapping macros
//
#define LDR_IS_DATAFILE(handle)     (((ULONG_PTR)(handle)) & (ULONG_PTR)1)
#define LDR_IS_IMAGEMAPPING(handle) (((ULONG_PTR)(handle)) & (ULONG_PTR)2)
#define LDR_IS_RESOURCE(handle)     (LDR_IS_IMAGEMAPPING(handle) || LDR_IS_DATAFILE(handle))

#define LDR_LOADCOUNT_MAX 0xFFFFFFFFul

//
// Activation Context
//
typedef PVOID PACTIVATION_CONTEXT;

typedef enum _LDR_DDAG_STATE
{
    LdrModulesMerged = -5,
    LdrModulesInitError = -4,
    LdrModulesSnapError = -3,
    LdrModulesUnloaded = -2,
    LdrModulesUnloading = -1,
    LdrModulesPlaceHolder = 0,
    LdrModulesMapping = 1,
    LdrModulesMapped = 2,
    LdrModulesWaitingForDependencies = 3,
    LdrModulesSnapping = 4,
    LdrModulesSnapped = 5,
    LdrModulesCondensed = 6,
    LdrModulesReadyToInit = 7,
    LdrModulesInitializing = 8,
    LdrModulesReadyToRun = 9
} LDR_DDAG_STATE, *PLDR_DDAG_STATE;

typedef enum _LDR_DLL_LOAD_REASON
{
    LoadReasonStaticDependency = 0,
    LoadReasonStaticForwarderDependency = 1,
    LoadReasonDynamicForwarderDependency = 2,
    LoadReasonDelayloadDependency = 3,
    LoadReasonDynamicLoad = 4,
    LoadReasonAsImageLoad = 5,
    LoadReasonAsDataLoad = 6,
    LoadReasonEnclavePrimary = 7,
    LoadReasonEnclaveDependency = 8,
    LoadReasonUnknown = -1
} LDR_DLL_LOAD_REASON, *PLDR_DLL_LOAD_REASON;

typedef struct _LDRP_CSLIST
{
    PSINGLE_LIST_ENTRY Tail;
} LDRP_CSLIST, *PLDRP_CSLIST;

typedef struct _LDR_SERVICE_TAG_RECORD
{
    struct _LDR_SERVICE_TAG_RECORD* Next;
    ULONG32 ServiceTag;
} LDR_SERVICE_TAG_RECORD, *PLDR_SERVICE_TAG_RECORD;

typedef union _LDRP_PATH_SEARCH_OPTIONS
{
    ULONG32 Flags;

    struct
    {
        ULONG32 Unknown;
    };
} LDRP_PATH_SEARCH_OPTIONS, *PLDRP_PATH_SEARCH_OPTIONS;

typedef struct _LDRP_LOAD_CONTEXT LDRP_LOAD_CONTEXT, *PLDRP_LOAD_CONTEXT;

// Loader Distributed Dependency Graph Node (as in Windows Internals)
// DDAG likely stands for Distributed Dependency Acyclic Graph
typedef struct _LDR_DDAG_NODE
{
    LIST_ENTRY Modules;
    LDR_SERVICE_TAG_RECORD* ServiceTagList;
    ULONG32 LoadCount;
    ULONG32 LoadWhileUnloadingCount;
    ULONG32 LowestLink;
    LDRP_CSLIST Dependencies;
    LDRP_CSLIST IncomingDependencies;
    LDR_DDAG_STATE State;
    SINGLE_LIST_ENTRY CondenseLink;
    ULONG32 PreorderNumber;
} LDR_DDAG_NODE, *PLDR_DDAG_NODE;

//
// Loader Data Table Entry
//
typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;

    union
    {
        ULONG32 Flags;

        struct
        {
            ULONG32 PackagedBinary : 1; // 0
            ULONG32 MarkedForRemoval : 1; // 1
            ULONG32 ImageDll : 1; // 2
            ULONG32 LoadNotificationsSent : 1; // 3
            ULONG32 TelemetryEntryProcessed : 1; // 4
            ULONG32 ProcessStaticImport : 1; // 5
            ULONG32 InLegacyLists : 1; // 6
            ULONG32 InIndexes : 1; // 7
            ULONG32 ShimDll : 1; // 8
            ULONG32 InExceptionTable : 1; // 9
            ULONG32 ReservedFlags1 : 1; // 10
            ULONG32 ReactOSSystemMapped : 1; // 11
            ULONG32 LoadInProgress : 1; // 12
            ULONG32 LoadConfigProcessed : 1; // 13
            ULONG32 EntryProcessed : 1; // 14
            ULONG32 ProtectDelayLoad : 1; // 15
            ULONG32 ReactOSLdrSymbolsLoaded : 1; // 16
            ULONG32 ReactOSDriverDependency : 1; // 17
            ULONG32 DontCallForThreads : 1; // 18
            ULONG32 ProcessAttachCalled : 1; // 19
            ULONG32 ProcessAttachFailed : 1; // 20
            ULONG32 CorDeferredValidate : 1; // 21
            ULONG32 CorImage : 1; // 22
            ULONG32 DontRelocate : 1; // 23
            ULONG32 CorILOnly : 1; // 24
            ULONG32 ChpeImage : 1; // 25; CHPE = Compiled Hybrid Portable Executable
            ULONG32 ReactOSDriverVerifying : 1; // 26
            ULONG32 ReactOSNativeMapped : 1; // 27
            ULONG32 Redirected : 1; // 28
            ULONG32 ReactOSShimSuppress : 1; // 29
            ULONG32 ReactOSKernelLoaded : 1; // 30
            ULONG32 CompatDatabaseProcessed : 1; // 31
        };
    };

    UINT16 ObsoleteLoadCount;
    UINT16 TlsIndex;
    LIST_ENTRY HashLinks;
    ULONG32 TimeDateStamp;
    PACTIVATION_CONTEXT EntryPointActivationContext;
    VOID* Lock;
    LDR_DDAG_NODE* DdagNode;
    LIST_ENTRY NodeModuleLink; // LDR_DDAG_NODE.Modules
    PLDRP_LOAD_CONTEXT LoadContext;
    VOID* ParentDllBase;
    VOID* SwitchBackContext;
    RTL_BALANCED_NODE BaseAddressIndexNode;
    RTL_BALANCED_NODE MappingInfoIndexNode;
    UINT_PTR OriginalBase;
    LARGE_INTEGER LoadTime;
    ULONG32 BaseNameHashValue;
    LDR_DLL_LOAD_REASON LoadReason;
    LDRP_PATH_SEARCH_OPTIONS ImplicitPathOptions;
    ULONG32 ReferenceCount;
    ULONG32 DependentLoadFlags;
    UINT8 SigningLevel;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;


typedef struct _LDRP_UNICODE_STRING_BUNDLE
{
    UNICODE_STRING String;
    WCHAR StaticBuffer[128];
} LDRP_UNICODE_STRING_BUNDLE, * PLDRP_UNICODE_STRING_BUNDLE;


//
// Loader Functions
//

NTSTATUS
NTAPI
LdrAddRefDll(
    _In_ ULONG Flags,
    _In_ PVOID BaseAddress
);

NTSTATUS
NTAPI
LdrDisableThreadCalloutsForDll(
    _In_ PVOID BaseAddress
);

NTSTATUS
NTAPI
LdrGetDllHandle(
    _In_opt_ PWSTR DllPath,
    _In_ PULONG DllCharacteristics,
    _In_ PUNICODE_STRING DllName,
    _Out_ PVOID* DllHandle
);

NTSTATUS
NTAPI
LdrGetDllHandleEx(
    _In_ ULONG Flags,
    _In_opt_ PWSTR DllPath,
    _In_opt_ PULONG DllCharacteristics,
    _In_ PUNICODE_STRING DllName,
    _Out_opt_ PVOID* DllHandle);

NTSTATUS
NTAPI
LdrFindEntryForAddress(
    _In_ PVOID Address,
    _Out_ PLDR_DATA_TABLE_ENTRY* Module
);

NTSTATUS
NTAPI
LdrGetProcedureAddress(
    _In_ PVOID BaseAddress,
    _In_opt_ PANSI_STRING Name,
    _In_opt_ ULONG Ordinal,
    _Out_ PVOID* ProcedureAddress
);

NTSTATUS
NTAPI
LdrGetProcedureAddressEx(
    _In_ PVOID BaseAddress,
    _In_opt_ PANSI_STRING FunctionName,
    _In_opt_ ULONG Ordinal,
    _Out_ PVOID* ProcedureAddress,
    _In_ UINT8 Flags
);

NTSTATUS
NTAPI
LdrGetProcedureAddressForCaller(
    _In_ PVOID BaseAddress,
    _In_opt_ PANSI_STRING FunctionName,
    _In_opt_ ULONG Ordinal,
    _Out_ PVOID* ProcedureAddress,
    _In_ UINT8 Flags,
    _In_ PVOID* CallbackAddress
);

NTSTATUS
NTAPI
LdrLoadDll(
    _In_opt_ PWSTR SearchPath,
    _In_opt_ PULONG LoadFlags,
    _In_ PUNICODE_STRING Name,
    _Out_opt_ PVOID* BaseAddress
);

PIMAGE_BASE_RELOCATION
NTAPI
LdrProcessRelocationBlock(
    _In_ ULONG_PTR Address,
    _In_ ULONG Count,
    _In_ PUSHORT TypeOffset,
    _In_ LONG_PTR Delta
);

NTSTATUS
NTAPI
LdrQueryImageFileExecutionOptions(
    _In_ PUNICODE_STRING SubKey,
    _In_ PCWSTR ValueName,
    _In_ ULONG ValueSize,
    _Out_ PVOID Buffer,
    _In_ ULONG BufferSize,
    _Out_opt_ PULONG ReturnedLength
);

#if 0
VOID
NTAPI
LdrSetDllManifestProber(
    _In_ PLDR_MANIFEST_PROBER_ROUTINE Routine);
#endif

NTSTATUS
NTAPI
LdrShutdownProcess(
    VOID);

NTSTATUS
NTAPI
LdrShutdownThread(
    VOID);

NTSTATUS
NTAPI
LdrUnloadDll(
    _In_ PVOID BaseAddress
);

typedef VOID (NTAPI* PLDR_CALLBACK)(PVOID CallbackContext, PCHAR Name);
NTSTATUS
NTAPI
LdrVerifyImageMatchesChecksum(
    _In_ HANDLE FileHandle,
    _In_ PLDR_CALLBACK Callback,
    _In_ PVOID CallbackContext,
    _Out_ PUSHORT ImageCharacteristics
);

NTSTATUS
NTAPI
LdrOpenImageFileOptionsKey(
    _In_ PUNICODE_STRING SubKey,
    _In_ BOOLEAN Wow64,
    _Out_ PHANDLE NewKeyHandle
);

NTSTATUS
NTAPI
LdrQueryImageFileKeyOption(
    _In_ HANDLE KeyHandle,
    _In_ PCWSTR ValueName,
    _In_ ULONG Type,
    _Out_ PVOID Buffer,
    _In_ ULONG BufferSize,
    _Out_opt_ PULONG ReturnedLength
);

//
// Resource Functions
//
NTSTATUS
NTAPI
LdrAccessResource(
    _In_ PVOID BaseAddress,
    _In_ PIMAGE_RESOURCE_DATA_ENTRY ResourceDataEntry,
    _Out_opt_ PVOID* Resource,
    _Out_opt_ PULONG Size
);

#if 0
NTSTATUS
NTAPI
LdrFindResource_U(
    _In_ PVOID BaseAddress,
    _In_ PLDR_RESOURCE_INFO ResourceInfo,
    _In_ ULONG Level,
    _Out_ PIMAGE_RESOURCE_DATA_ENTRY* ResourceDataEntry
);

NTSTATUS
NTAPI
LdrEnumResources(
    _In_ PVOID BaseAddress,
    _In_ PLDR_RESOURCE_INFO ResourceInfo,
    _In_ ULONG Level,
    _Inout_ ULONG* ResourceCount,
    _Out_writes_to_(*ResourceCount, *ResourceCount) LDR_ENUM_RESOURCE_INFO* Resources
);


NTSTATUS
NTAPI
LdrFindResourceDirectory_U(
    _In_ PVOID BaseAddress,
    _In_ PLDR_RESOURCE_INFO ResourceInfo,
    _In_ ULONG Level,
    _Out_ PIMAGE_RESOURCE_DIRECTORY* ResourceDirectory
);
#endif

NTSTATUS
NTAPI
LdrLoadAlternateResourceModule(
    _In_ PVOID Module,
    _In_ PWSTR Buffer
);

BOOLEAN
NTAPI
LdrUnloadAlternateResourceModule(
    _In_ PVOID BaseAddress
);

//
// Misc. Functions
//

NTSTATUS
NTAPI
LdrLockLoaderLock(
    _In_ ULONG Flags,
    _Out_opt_ PULONG Disposition,
    _Out_opt_ PULONG_PTR Cookie
);

NTSTATUS
NTAPI
LdrUnlockLoaderLock(
    _In_ ULONG Flags,
    _In_opt_ ULONG_PTR Cookie
);

BOOLEAN
NTAPI
LdrVerifyMappedImageMatchesChecksum(
    _In_ PVOID BaseAddress,
    _In_ SIZE_T NumberOfBytes,
    _In_ ULONG FileLength
);

PIMAGE_BASE_RELOCATION
NTAPI
LdrProcessRelocationBlockLongLong(
    _In_ ULONG_PTR Address,
    _In_ ULONG Count,
    _In_ PUSHORT TypeOffset,
    _In_ LONGLONG Delta
);

// Callback function for LdrEnumerateLoadedModules
typedef VOID (NTAPI LDR_ENUM_CALLBACK)(_In_ PLDR_DATA_TABLE_ENTRY ModuleInformation, _In_ PVOID Parameter,
                                       _Out_ BOOLEAN* Stop);
typedef LDR_ENUM_CALLBACK* PLDR_ENUM_CALLBACK;

NTSTATUS
NTAPI
LdrEnumerateLoadedModules(
    _In_ BOOLEAN ReservedFlag,
    _In_ PLDR_ENUM_CALLBACK EnumProc,
    _In_ PVOID Context
);

#ifndef NTOS_MODE_USER
typedef struct _SYSTEM_MODULE_INFORMATION LDR_PROCESS_MODULES, * PLDR_PROCESS_MODULES;
#else
typedef struct _RTL_PROCESS_MODULES LDR_PROCESS_MODULES, *PLDR_PROCESS_MODULES;
#endif

NTSTATUS
NTAPI
LdrQueryProcessModuleInformation(
    _In_opt_ PLDR_PROCESS_MODULES ModuleInformation,
    _In_opt_ ULONG Size,
    _Out_ PULONG ReturnedSize
);

#define LDR_HASH_TABLE_ENTRIES 32
#define LDR_GET_HASH_ENTRY(x) ((x) & (LDR_HASH_TABLE_ENTRIES - 1))

/* Loader flags */
#define IMAGE_LOADER_FLAGS_COMPLUS 0x00000001
#define IMAGE_LOADER_FLAGS_SYSTEM_GLOBAL 0x01000000

/* Page heap flags */
#define DPH_FLAG_DLL_NOTIFY 0x40

typedef struct _LDRP_PATH_SEARCH_CONTEXT
{
    UNICODE_STRING DllSearchPath;
    BOOLEAN AllocatedOnLdrpHeap;
    LDRP_PATH_SEARCH_OPTIONS SearchOptions;
    UNICODE_STRING OriginalFullDllName; // e.g. for forwarders
} LDRP_PATH_SEARCH_CONTEXT, *PLDRP_PATH_SEARCH_CONTEXT;

typedef union _LDRP_LOAD_CONTEXT_FLAGS
{
    ULONG32 Flags;

    struct
    {
        ULONG32 Redirected : 1;
        ULONG32 Static : 1;
        ULONG32 BaseNameOnly : 1;
        ULONG32 HasFullPath : 1;
        ULONG32 KnownDll : 1;
        ULONG32 SystemImage : 1;
        ULONG32 ExecutableImage : 1;
        ULONG32 AppContainerImage : 1;
        ULONG32 CallInit : 1;
        ULONG32 UserAllocated : 1;
        ULONG32 SearchOnlyFirstPathSegment : 1;
        ULONG32 RedirectedByAPISet : 1;
    };
} LDRP_LOAD_CONTEXT_FLAGS, *PLDRP_LOAD_CONTEXT_FLAGS;

typedef struct _LDRP_LOAD_CONTEXT
{
    UNICODE_STRING DllName;
    PLDRP_PATH_SEARCH_CONTEXT PathSearchContext;
    UINT_PTR DllSectionHandle;
    LDRP_LOAD_CONTEXT_FLAGS Flags;
    NTSTATUS* StatusResponse;
    PLDR_DATA_TABLE_ENTRY ParentEntry;
    PLDR_DATA_TABLE_ENTRY Entry;
    LIST_ENTRY WorkQueueListEntry;
    PLDR_DATA_TABLE_ENTRY PendingDependencyEntry;
    PLDR_DATA_TABLE_ENTRY* ImportEntries;
    ULONG ImportEntriesCount;
    ULONG32 CountOfDependenciesPendingMap;
    PVOID BaseAddressOfIAT;
    UINT_PTR SizeOfIAT;
    ULONG SnappedImportEntriesCount; // 0 <= SnappedImportEntriesCount <= ImportEntriesCount
    PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor;
    ULONG OriginalIATProtection;
    PVOID GuardCFCheckFunctionPointer;
    PVOID* GuardCFCheckFunctionPointerThunk;
    UINT_PTR DllMappedSectionSize;
    UINT_PTR EnclaveContext;
    UINT_PTR DllMappedFileHandle;
    UINT_PTR ProcessImageSectionViewBase;
    WCHAR DllNameStorage[ANYSIZE_ARRAY];
} LDRP_LOAD_CONTEXT, *PLDRP_LOAD_CONTEXT;

typedef struct _TLS_VECTOR
{
    union
    {
        ULONG Length;
        HANDLE ThreadId;
    };

    struct _TLS_VECTOR* PreviousDeferredTlsVector;
    PVOID ModuleTlsData[ANYSIZE_ARRAY];
} TLS_VECTOR, *PTLS_VECTOR;

typedef struct _LDRP_LAZY_PATH_EVALUATION_CONTEXT
{
    UNICODE_STRING RemainingSearchPath;
} LDRP_LAZY_PATH_EVALUATION_CONTEXT, *PLDRP_LAZY_PATH_EVALUATION_CONTEXT;

FORCEINLINE
VOID
FatalListEntryError(
    _In_ PVOID P1,
    _In_ PVOID P2,
    _In_ PVOID P3)
{
    UNREFERENCED_PARAMETER(P1);
    UNREFERENCED_PARAMETER(P2);
    UNREFERENCED_PARAMETER(P3);

    __fastfail(3);
}

FORCEINLINE
VOID
RtlpCheckListEntry(
    _In_ PLIST_ENTRY Entry)
{
    if (Entry->Flink->Blink != Entry || Entry->Blink->Flink != Entry)
        FatalListEntryError(Entry->Blink, Entry, Entry->Flink);
}

/* Global data */
extern PVOID LdrpSystemDllBase;
extern PVOID LdrpHeap;
extern HANDLE LdrpKnownDllDirectoryHandle;
extern PLDR_DATA_TABLE_ENTRY LdrpNtDllDataTableEntry;
extern BOOLEAN UseCOR;
extern PVOID LdrpMscoreeDllHandle;
extern RTL_SRWLOCK LdrpModuleDatatableLock;
extern LIST_ENTRY LdrpWorkQueue;
extern LIST_ENTRY LdrpRetryQueue;
extern RTL_CRITICAL_SECTION LdrpWorkQueueLock;
extern PLDR_DATA_TABLE_ENTRY LdrpImageEntry;
#if 0
extern RTL_CRITICAL_SECTION LdrpLoaderLock;
extern BOOLEAN LdrpInLdrInit;
extern LIST_ENTRY LdrpHashTable[LDR_HASH_TABLE_ENTRIES];
extern BOOLEAN ShowSnaps;
extern UNICODE_STRING LdrpDefaultPath;
extern ULONG LdrpNumberOfProcessors;
extern ULONG LdrpFatalHardErrorCount;
extern PUNICODE_STRING LdrpTopLevelDllBeingLoaded;
extern PLDR_DATA_TABLE_ENTRY LdrpCurrentDllInitializer;
extern UNICODE_STRING LdrpDefaultExtension;
extern BOOLEAN LdrpShutdownInProgress;
extern UNICODE_STRING LdrpKnownDllPath;
extern PLDR_DATA_TABLE_ENTRY LdrpLoadedDllHandleCache;
extern BOOLEAN RtlpPageHeapEnabled;
extern ULONG RtlpDphGlobalFlags;
extern BOOLEAN g_ShimsEnabled;
extern PVOID g_pShimEngineModule;
extern PVOID g_pfnSE_DllLoaded;
extern PVOID g_pfnSE_DllUnloaded;
extern PVOID g_pfnSE_InstallBeforeInit;
extern PVOID g_pfnSE_InstallAfterInit;
extern PVOID g_pfnSE_ProcessDying;
extern PCONTEXT LdrpProcessInitContextRecord;
#endif
#if 0
extern PLDR_MANIFEST_PROBER_ROUTINE LdrpManifestProberRoutine;
#endif

extern LIST_ENTRY LdrpTlsList;
#define TLS_BITMAP_GROW_INCREMENT 8u
#if 0
extern RTL_BITMAP LdrpTlsBitmap;
#endif
extern ULONG LdrpActiveThreadCount;
extern ULONG LdrpActualBitmapSize;
extern RTL_SRWLOCK LdrpTlsLock;

typedef union
{
    ULONG32 Flags;

    struct
    {
        ULONG32 LogWarning : 1;
        ULONG32 LogInformation : 1;
        ULONG32 LogDebug : 1;
        ULONG32 LogTrace : 1;
        ULONG32 BreakInDebugger : 1;
    };
} LDRP_DEBUG_FLAGS;

extern LDRP_DEBUG_FLAGS LdrpDebugFlags;

/* ldrtls.c */
typedef struct _TLS_ENTRY
{
    LIST_ENTRY TlsEntryLinks;
    IMAGE_TLS_DIRECTORY TlsDirectory;
    PLDR_DATA_TABLE_ENTRY ModuleEntry;
} TLS_ENTRY, *PTLS_ENTRY;

PTLS_ENTRY
FASTCALL
LdrpFindTlsEntry(
    IN PLDR_DATA_TABLE_ENTRY ModuleEntry
);

#define LDRP_BITMAP_BITALIGN (sizeof(ULONG)*8)
#define LDRP_BITMAP_CALC_ALIGN(x, base) (((x) + (base) - 1) / (base))

#define PTR_ADD_OFFSET(Pointer, Offset) ((PVOID)((ULONG_PTR)(Pointer) + (ULONG_PTR)(Offset)))
#define PTR_SUB_OFFSET(Pointer, Offset) ((PVOID)((ULONG_PTR)(Pointer) - (ULONG_PTR)(Offset)))

// undocumented LDRP DDAG storage class
typedef struct _LDRP_CSLIST_ENTRY
{
    SINGLE_LIST_ENTRY DependenciesLink;
    struct _LDR_DDAG_NODE* DependencyNode;
    SINGLE_LIST_ENTRY IncomingDependenciesLink;
    struct _LDR_DDAG_NODE* ParentNode;
} LDRP_CSLIST_ENTRY, *PLDRP_CSLIST_ENTRY;

NTSTATUS
NTAPI
LdrpReleaseTlsEntry(
    IN PLDR_DATA_TABLE_ENTRY ModuleEntry,
    OUT PTLS_ENTRY* FoundTlsEntry OPTIONAL
);

PLDR_DATA_TABLE_ENTRY
NTAPI
LdrpAllocateModuleEntry(IN PLDRP_LOAD_CONTEXT LoadContext OPTIONAL);

NTSTATUS
NTAPI
LdrpAllocatePlaceHolder(IN PUNICODE_STRING DllName,
                        IN PLDRP_PATH_SEARCH_CONTEXT PathSearchContext,
                        IN LDRP_LOAD_CONTEXT_FLAGS Flags,
                        IN LDR_DLL_LOAD_REASON LoadReason,
                        IN PLDR_DATA_TABLE_ENTRY ParentEntry,
                        OUT PLDR_DATA_TABLE_ENTRY* OutLdrEntry,
                        IN NTSTATUS* OutStatus);

NTSTATUS
NTAPI
LdrpPrepareModuleForExecution(IN PLDR_DATA_TABLE_ENTRY LdrEntry,
                              IN NTSTATUS* StatusResponse);

PVOID
NTAPI
LdrpInitSecurityCookie(PLDR_DATA_TABLE_ENTRY LdrEntry);

NTSTATUS
NTAPI
LdrpMapAndSnapDependency(PLDRP_LOAD_CONTEXT LoadContext);

ULONG32
NTAPI
LdrpHashUnicodeString(IN PUNICODE_STRING NameString);

VOID
NTAPI
LdrpDrainWorkQueue(VOID);

VOID
NTAPI
LdrpQueueWork(IN PLDRP_LOAD_CONTEXT LoadContext);

NTSTATUS
NTAPI
LdrpIncrementModuleLoadCount(IN PLDR_DATA_TABLE_ENTRY LdrEntry);

NTSTATUS
NTAPI
LdrpDoPostSnapWork(IN PLDRP_LOAD_CONTEXT LoadContext);

NTSTATUS
NTAPI
LdrpProcessMappedModule(PLDR_DATA_TABLE_ENTRY LdrEntry, LDRP_LOAD_CONTEXT_FLAGS LoadContextFlags, BOOL AdvanceLoad);

NTSTATUS
NTAPI
LdrpResolveDllName(
    IN PUNICODE_STRING DllName,
    OUT PUNICODE_STRING FullDosDllName,
    OUT PUNICODE_STRING FullNtDllName,
    OUT PUNICODE_STRING BaseDllName OPTIONAL);

NTSTATUS
NTAPI
LdrpSearchPath(IN PUNICODE_STRING DllName,
               IN PLDRP_PATH_SEARCH_CONTEXT PathSearchContext,
               IN BOOLEAN SearchInFirstSegmentOnly,
               IN OUT PLDRP_LAZY_PATH_EVALUATION_CONTEXT* LazyEvaluationContext OPTIONAL,
               OUT PUNICODE_STRING ExpandedDosName OPTIONAL,
               OUT PUNICODE_STRING ExpandedNtName OPTIONAL,
               OUT PUNICODE_STRING BaseDllName OPTIONAL);


NTSTATUS
NTAPI
LdrpFindLoadedDllByNameLockHeld(IN PUNICODE_STRING BaseDllName,
                                IN PUNICODE_STRING FullDllName OPTIONAL,
                                IN LDRP_LOAD_CONTEXT_FLAGS LoadContextFlags OPTIONAL,
                                IN ULONG32 BaseNameHashValue,
                                OUT PLDR_DATA_TABLE_ENTRY* LdrEntry);

NTSTATUS
NTAPI
LdrpFindLoadedDllByName(IN PUNICODE_STRING BaseDllName OPTIONAL,
                        IN PUNICODE_STRING FullDllName OPTIONAL,
                        IN LDRP_LOAD_CONTEXT_FLAGS LoadContextFlags OPTIONAL,
                        OUT PLDR_DATA_TABLE_ENTRY* LdrEntry,
                        OUT LDR_DDAG_STATE* DdagState OPTIONAL);

NTSTATUS
NTAPI
LdrpFindExistingModule(IN PUNICODE_STRING BaseDllName OPTIONAL,
                       IN PUNICODE_STRING FullDllName OPTIONAL,
                       IN LDRP_LOAD_CONTEXT_FLAGS LoadContextFlags OPTIONAL,
                       IN ULONG32 BaseNameHashValue,
                       OUT PLDR_DATA_TABLE_ENTRY* LdrEntry);

NTSTATUS
NTAPI
LdrpPreprocessDllName(IN PUNICODE_STRING DllName,
                      IN OUT PLDRP_UNICODE_STRING_BUNDLE OutputDllName,
                      IN PLDR_DATA_TABLE_ENTRY ParentEntry OPTIONAL,
                      OUT PLDRP_LOAD_CONTEXT_FLAGS LoadContextFlags);

NTSTATUS NTAPI LdrpInitializeTls(VOID);
NTSTATUS NTAPI LdrpAllocateTls(VOID);
VOID NTAPI LdrpFreeTls(VOID);
NTSTATUS NTAPI LdrpHandleTlsData(IN OUT PLDR_DATA_TABLE_ENTRY ModuleEntry);

/* ldrcor.c */
NTSTATUS
NTAPI
LdrpCorInitialize(OUT PLDR_DATA_TABLE_ENTRY* TargetEntry);

typedef __int32 (STDMETHODCALLTYPE LDRP_COREXEMAIN_FUNC)();

BOOL
NTAPI
LdrpIsILOnlyImage(PVOID BaseAddress);

NTSTATUS
NTAPI
LdrpCorValidateImage(IN PVOID ImageBase, IN LPCWSTR FileName);

NTSTATUS
NTAPI
LdrpCorProcessImports(IN PLDR_DATA_TABLE_ENTRY LdrEntry);

/* ldrinit.c */
VOID NTAPI LdrpEnsureLoaderLockIsHeld(VOID);

/* ldrpe.c */
NTSTATUS
NTAPI
LdrpBuildSystem32FileName(
    IN PUNICODE_STRING DestinationString,
    IN PUNICODE_STRING FileName OPTIONAL);

NTSTATUS
NTAPI
LdrpSnapThunk(IN PLDR_DATA_TABLE_ENTRY ExportEntry,
              IN PLDR_DATA_TABLE_ENTRY ImportEntry,
              IN PIMAGE_THUNK_DATA OriginalThunk,
              IN OUT PIMAGE_THUNK_DATA Thunk);

NTSTATUS
NTAPI
LdrpSnapModule(IN PLDRP_LOAD_CONTEXT LoadContext);


/* ldrutils.c */

NTSTATUS
NTAPI
LdrpGetProcedureAddress(IN PVOID BaseAddress,
                        IN PANSI_STRING Name,
                        IN ULONG Ordinal,
                        OUT PVOID* ProcedureAddress);

VOID NTAPI
LdrpInsertMemoryTableEntry(IN PLDR_DATA_TABLE_ENTRY LdrEntry);

NTSTATUS
NTAPI
LdrpLoadDll(IN PUNICODE_STRING RawDllName,
            IN PLDRP_PATH_SEARCH_CONTEXT PathSearchContext,
            IN LDRP_LOAD_CONTEXT_FLAGS LoaderFlags,
            OUT PLDR_DATA_TABLE_ENTRY* OutputLdrEntry);

NTSTATUS
NTAPI
LdrpProcessWork(PLDRP_LOAD_CONTEXT LoadContext);

ULONG NTAPI
LdrpClearLoadInProgress(VOID);

NTSTATUS
NTAPI
LdrpSetProtection(PVOID ViewBase,
                  BOOLEAN Restore);


ULONG
NTAPI
LdrpRelocateImage(IN PVOID NewBase,
                  IN PCCH LoaderName,
                  IN PIMAGE_NT_HEADERS NtHeaders,
                  IN ULONG Success,
                  IN ULONG Conflict,
                  IN ULONG Invalid);
ULONG
NTAPI
LdrpProtectAndRelocateImage(IN PVOID NewBase,
                            IN PCCH LoaderName,
                            IN ULONG Success,
                            IN ULONG Conflict,
                            IN ULONG Invalid);

NTSTATUS
NTAPI
LdrpFindLoadedDllByAddress(IN PVOID Base,
                           OUT PLDR_DATA_TABLE_ENTRY* LdrEntry,
                           LDR_DDAG_STATE* DdagState);
NTSTATUS
NTAPI
LdrpFindLoadedDllByHandle(IN PVOID Base,
                          OUT PLDR_DATA_TABLE_ENTRY* LdrEntry,
                          LDR_DDAG_STATE* DdagState);

#if 0
BOOLEAN NTAPI
LdrpCheckForLoadedDll_Legacy(IN PWSTR DllPath,
    IN PUNICODE_STRING DllName,
    IN BOOLEAN Flag,
    IN BOOLEAN RedirectedDll,
    OUT PLDR_DATA_TABLE_ENTRY * LdrEntry);
#endif

NTSTATUS
NTAPI
LdrpApplyFileNameRedirection(IN PLDR_DATA_TABLE_ENTRY ParentEntry,
                             IN PUNICODE_STRING DllName,
                             IN PVOID,
                             IN OUT PLDRP_UNICODE_STRING_BUNDLE RedirectedDllName,
                             OUT PBOOLEAN RedirectedSxS);

NTSTATUS
NTAPI
LdrpLoadKnownDll(IN PLDRP_LOAD_CONTEXT LoadContext);

NTSTATUS
NTAPI
LdrpMapDllNtFileName(IN PLDRP_LOAD_CONTEXT LoadContext,
                     IN PUNICODE_STRING NtPathDllName);

NTSTATUS
NTAPI
LdrpMapDllFullPath(IN PLDRP_LOAD_CONTEXT LoadContext);

NTSTATUS
NTAPI
LdrpMapDllSearchPath(IN PLDRP_LOAD_CONTEXT LoadContext);

VOID NTAPI
LdrpGetShimEngineInterface(VOID);

VOID
NTAPI
LdrpLoadShimEngine(IN PWSTR ImageName,
                   IN PUNICODE_STRING ProcessImage,
                   IN PVOID pShimData);

VOID NTAPI
LdrpUnloadShimEngine(VOID);

NTSTATUS
NTAPI
LdrpCodeAuthzCheckDllAllowed(IN PUNICODE_STRING FullName,
                             IN HANDLE DllHandle);

LDRP_LOAD_CONTEXT_FLAGS
NTAPI
LdrpDllCharacteristicsToLoadFlags(ULONG DllCharacteristics);

NTSTATUS
NTAPI
LdrpFindOrPrepareLoadingModule(IN PUNICODE_STRING BaseDllName,
                               IN PLDRP_PATH_SEARCH_CONTEXT PathSearchContext,
                               IN LDRP_LOAD_CONTEXT_FLAGS ContextFlags,
                               IN LDR_DLL_LOAD_REASON LoadReason,
                               IN PLDR_DATA_TABLE_ENTRY ParentEntry,
                               OUT PLDR_DATA_TABLE_ENTRY* OutLdrEntry,
                               OUT NTSTATUS* OutStatus);

VOID
NTAPI
LdrpLoadContextReplaceModule(IN PLDRP_LOAD_CONTEXT LoadContext, PLDR_DATA_TABLE_ENTRY LoadedEntry);

/* verifier.c */

#if 0
NTSTATUS NTAPI
LdrpInitializeApplicationVerifierPackage(IN HANDLE KeyHandle,
                                         IN PPEB Peb,
                                         IN BOOLEAN SystemWide,
                                         IN BOOLEAN ReadAdvancedOptions);
#endif

NTSTATUS NTAPI
AVrfInitializeVerifier(VOID);

VOID NTAPI
AVrfDllLoadNotification(IN PLDR_DATA_TABLE_ENTRY LdrEntry);

VOID NTAPI
AVrfDllUnloadNotification(IN PLDR_DATA_TABLE_ENTRY LdrEntry);

VOID NTAPI
AVrfPageHeapDllNotification(IN PLDR_DATA_TABLE_ENTRY LdrEntry);


/* FIXME: Cleanup this mess */
#if 0
typedef NTSTATUS (NTAPI* PEPFUNC)(PPEB);
#endif
NTSTATUS LdrMapSections(HANDLE ProcessHandle,
                        PVOID ImageBase,
                        HANDLE SectionHandle,
                        PIMAGE_NT_HEADERS NTHeaders);
NTSTATUS LdrMapNTDllForProcess(HANDLE ProcessHandle,
                               PHANDLE NTDllSectionHandle);
ULONG
LdrpGetResidentSize(PIMAGE_NT_HEADERS NTHeaders);

NTSTATUS
NTAPI
LdrpLoadDependentModule(IN PANSI_STRING RawDllName,
                        IN PLDRP_LOAD_CONTEXT LoadContext,
                        IN PLDR_DATA_TABLE_ENTRY ParentLdrEntry,
                        IN BOOLEAN ForwardedDll,
                        OUT PLDR_DATA_TABLE_ENTRY* DataTableEntry,
                        IN OUT PLDRP_CSLIST_ENTRY* LastDependencyEntry);

VOID
NTAPI
LdrpFinalizeAndDeallocateDataTableEntry(IN PLDR_DATA_TABLE_ENTRY Entry);

NTSTATUS
NTAPI
LdrpUnmapModule(IN PLDR_DATA_TABLE_ENTRY LdrEntry);

void
NTAPI
LdrpDereferenceModule(IN PLDR_DATA_TABLE_ENTRY LdrEntry);


NTSTATUS
NTAPI
LdrpDecrementNodeLoadCountLockHeld(IN PLDR_DDAG_NODE Node, IN BOOLEAN DisallowOrphaning, OUT BOOLEAN* BecameOrphan);

NTSTATUS
NTAPI
LdrpDecrementNodeLoadCountEx(IN PLDR_DATA_TABLE_ENTRY LdrEntry, IN BOOLEAN DisallowOrphaning);

NTSTATUS
NTAPI
LdrpUnloadNode(IN PLDR_DDAG_NODE Node);

NTSTATUS
NTAPI
LdrpPinModule(IN PLDR_DATA_TABLE_ENTRY LdrEntry);

NTSTATUS
NTAPI
LdrpGetFullPath(IN PUNICODE_STRING OriginalName,
                IN OUT PUNICODE_STRING ExpandedName);

NTSTATUS
NTAPI
LdrpInitializeNode(IN PLDR_DDAG_NODE DdagNode);

NTSTATUS
NTAPI
// ReSharper disable once IdentifierTypo
LdrpFastpthReloadedDll(IN PUNICODE_STRING BaseDllName,
                       IN LDRP_LOAD_CONTEXT_FLAGS LoadContextFlags,
                       IN PLDR_DATA_TABLE_ENTRY ForwarderSource OPTIONAL,
                       OUT PLDR_DATA_TABLE_ENTRY* OutputLdrEntry);

NTSTATUS
NTAPI
LdrpMinimalMapModule(PLDRP_LOAD_CONTEXT LoadContext, HANDLE SectionHandle, PSIZE_T ViewSize);

NTSTATUS
NTAPI
LdrpCompleteMapModule(PLDRP_LOAD_CONTEXT LoadContext, PIMAGE_NT_HEADERS NtHeaders, NTSTATUS ImageStatus,
                      SIZE_T ViewSize);

NTSTATUS
NTAPI
LdrpRunInitializeRoutine(IN PLDR_DATA_TABLE_ENTRY LdrEntry);

void
NTAPI
LdrpLoadDllInternal(IN PUNICODE_STRING DllName,
                    IN PLDRP_PATH_SEARCH_CONTEXT PathSearchContext,
                    IN LDRP_LOAD_CONTEXT_FLAGS LoadContextFlags,
                    IN LDR_DLL_LOAD_REASON LoadReason,
                    IN PLDR_DATA_TABLE_ENTRY ParentEntry,
                    IN PLDR_DATA_TABLE_ENTRY ForwarderSource OPTIONAL,
                    OUT PLDR_DATA_TABLE_ENTRY* OutputLdrEntry,
                    OUT NTSTATUS* OutStatus);

NTSTATUS
NTAPI
LdrpFindKnownDll(PUNICODE_STRING DllName,
                 PUNICODE_STRING FullDllName,
                 PUNICODE_STRING BaseDllName,
                 HANDLE* SectionHandle);

NTSTATUS
NTAPI
LdrpMapDllWithSectionHandle(PLDRP_LOAD_CONTEXT LoadContext, HANDLE SectionHandle);


VOID
NTAPI
LdrpAddDependency(IN PLDR_DATA_TABLE_ENTRY ParentEntry, IN PLDR_DATA_TABLE_ENTRY ChildEntry);


NTSTATUS
NTAPI
LdrpSnapIAT(IN PLDR_DATA_TABLE_ENTRY ExportLdrEntry,
            IN PLDR_DATA_TABLE_ENTRY ImportLdrEntry,
            IN PIMAGE_IMPORT_DESCRIPTOR IatEntry);

NTSTATUS
NTAPI
LdrpInitializeGraphRecurse(IN PLDR_DDAG_NODE DdagNode,
                           IN NTSTATUS* StatusResponse,
                           IN OUT BOOLEAN* HasInitializing);


NTSTATUS
NTAPI
LdrpGetNtPathFromDosPath(IN PUNICODE_STRING DosPath,
                         OUT PUNICODE_STRING NtPath);


PVOID LdrpGetNewTlsVector(IN ULONG TlsBitmapLength);


NTSTATUS
LdrpAllocateTlsEntry(
    IN PIMAGE_TLS_DIRECTORY TlsDirectory,
    IN PLDR_DATA_TABLE_ENTRY ModuleEntry,
    OUT PULONG TlsIndex,
    OUT PBOOLEAN AllocatedBitmap OPTIONAL,
    OUT PTLS_ENTRY* TlsEntry
);

VOID
LdrpQueueDeferredTlsData(
    IN OUT PVOID TlsVector,
    IN OUT PVOID ThreadId
);

BOOLEAN
NTAPI
LdrpIsBaseNameOnly(IN PUNICODE_STRING DllName);


NTSTATUS
NTAPI
LdrpCreateDllSection(IN PUNICODE_STRING FullName,
                     IN HANDLE DllHandle,
                     IN LDRP_LOAD_CONTEXT_FLAGS LoaderFlags,
                     OUT PHANDLE SectionHandle);


UINT32
NTAPI
LdrpNameToOrdinal(IN LPSTR ImportName,
                  IN ULONG NumberOfNames,
                  IN PVOID ExportBase,
                  IN PULONG NameTable,
                  IN PUSHORT OrdinalTable);


NTSTATUS
NTAPI
LdrpHandleOldFormatImportDescriptors(IN PLDRP_LOAD_CONTEXT LoadContext,
                                     IN PLDR_DATA_TABLE_ENTRY LdrEntry,
                                     IN PIMAGE_IMPORT_DESCRIPTOR ImportEntry);


NTSTATUS
NTAPI
LdrpHandleNewFormatImportDescriptors(IN PLDRP_LOAD_CONTEXT LoadContext,
                                     IN PLDR_DATA_TABLE_ENTRY LdrEntry,
                                     IN PIMAGE_BOUND_IMPORT_DESCRIPTOR BoundEntry);


void
NTAPI
LdrpCondenseGraph(PLDR_DDAG_NODE Node);

void
NTAPI
LdrpFreeReplacedModule(PLDR_DATA_TABLE_ENTRY Entry);

void
NTAPI
LdrpFreeLoadContext(PLDRP_LOAD_CONTEXT LoadContext);

NTSTATUS
NTAPI
LdrpBuildForwarderLink(IN PLDR_DATA_TABLE_ENTRY Source OPTIONAL,
                       IN PLDR_DATA_TABLE_ENTRY Target);

PLDR_DATA_TABLE_ENTRY
NTAPI
LdrpHandleReplacedModule(PLDR_DATA_TABLE_ENTRY Entry);

NTSTATUS
NTAPI
LdrpFindLoadedDllByMapping(PVOID ViewBase, PIMAGE_NT_HEADERS NtHeader, PLDR_DATA_TABLE_ENTRY* LdrEntry,
                           LDR_DDAG_STATE* DdagState);

NTSTATUS
NTAPI
LdrpFindLoadedDllByMappingFile(PUNICODE_STRING NtPathName, PLDR_DATA_TABLE_ENTRY* LdrEntry, LDR_DDAG_STATE* DdagState);

NTSTATUS
NTAPI
LdrpFindLoadedDll(PUNICODE_STRING RawDllName,
                  PLDRP_PATH_SEARCH_CONTEXT PathSearchContext,
                  PLDR_DATA_TABLE_ENTRY* LdrEntry);

void
NTAPI
LdrpSignalModuleMapped(PLDR_DATA_TABLE_ENTRY LdrEntry);

BOOLEAN
NTAPI
LdrpIsExtensionPresent(IN PUNICODE_STRING DllName);

void
NTAPI
LdrpTrimTrailingDots(IN OUT PUNICODE_STRING DllName);

NTSTATUS
NTAPI
LdrpParseForwarderDescription(IN PVOID ForwarderPointer,
                              IN OUT PANSI_STRING ForwardedDllName,
                              IN OUT PANSI_STRING* ForwardedFunctionName,
                              IN OUT PULONG ForwardedFunctionOrdinal);

typedef union
{
    UINT8 Flags;

    struct
    {
        UINT8 DoSecurityVerification : 1;
        UINT8 NoForwarderResolution : 1;
    };
} LDRP_RESOLVE_PROCEDURE_ADDRESS_FLAGS;

NTSTATUS
NTAPI
LdrpResolveProcedureAddress(IN PLDR_DATA_TABLE_ENTRY LdrEntry,
                            IN PANSI_STRING Name,
                            IN ULONG Ordinal,
                            IN LDRP_RESOLVE_PROCEDURE_ADDRESS_FLAGS Flags,
                            OUT PVOID* ProcedureAddress);

NTSTATUS
NTAPI
LdrpLoadForwardedDll(IN PANSI_STRING RawDllName,
                     IN PLDRP_PATH_SEARCH_CONTEXT PathSearchContext,
                     IN PLDR_DATA_TABLE_ENTRY ForwarderSource,
                     IN PLDR_DATA_TABLE_ENTRY ParentEntry,
                     IN LDR_DLL_LOAD_REASON LoadReason,
                     OUT PLDR_DATA_TABLE_ENTRY* OutputLdrEntry);

void
NTAPI
LdrpReportSnapError(IN PVOID BaseAddress OPTIONAL,
                    IN PUNICODE_STRING ModuleName OPTIONAL,
                    IN PANSI_STRING FunctionName OPTIONAL,
                    IN ULONG Ordinal OPTIONAL,
                    NTSTATUS Status,
                    BOOLEAN Static);

NTSTATUS
NTAPI
LdrpResolveForwarder(IN PVOID ForwarderPointer,
                     IN PLDR_DATA_TABLE_ENTRY ExportLdrEntry,
                     IN PLDR_DATA_TABLE_ENTRY ImportLdrEntry,
                     OUT PVOID* ProcedureAddress);


_Must_inspect_result_
_Ret_maybenull_
_Post_writable_byte_size_(Size)
PVOID
NTAPI
LdrpHeapAlloc(_In_opt_ ULONG Flags,
              _In_ SIZE_T Size);

_Success_(return != 0)
BOOLEAN
NTAPI
LdrpHeapFree(_In_opt_ ULONG Flags,
             _In_ _Post_invalid_ PVOID BaseAddress);

_Must_inspect_result_
_Ret_maybenull_
_Post_writable_byte_size_(Size)
PVOID
NTAPI
LdrpHeapReAlloc(_In_opt_ ULONG Flags,
                _In_ _Post_invalid_ PVOID Ptr,
                _In_ SIZE_T Size);

NTSTATUS
NTAPI
LdrpAppendUnicodeStringToFilenameBuffer(UNICODE_STRING* Destination, UNICODE_STRING* Source);

NTSTATUS
NTAPI
LdrpAppendAnsiStringToFilenameBuffer(UNICODE_STRING* Destination, ANSI_STRING* Source);

NTSTATUS
NTAPI
LdrpAllocateFileNameBufferIfNeeded(UNICODE_STRING* Destination, ULONG Length);

void
NTAPI
LdrpFreeLoadContextOfNode(PLDR_DDAG_NODE Node, NTSTATUS* StatusResponse);

BOOLEAN
NTAPI
LdrpDependencyExist(PLDR_DDAG_NODE Node1, PLDR_DDAG_NODE Node2);

void
NTAPI
LdrpRecordModuleDependency(PLDR_DATA_TABLE_ENTRY LdrEntry1,
                           PLDR_DATA_TABLE_ENTRY LdrEntry2,
                           PLDRP_CSLIST_ENTRY Dependency,
                           NTSTATUS* StatusResponse);

PIMAGE_LOAD_CONFIG_DIRECTORY
NTAPI
LdrImageDirectoryEntryToLoadConfig(PVOID DllBase);

PIMAGE_IMPORT_DESCRIPTOR
NTAPI
LdrpGetImportDescriptorForSnap(PLDRP_LOAD_CONTEXT LoadContext);

NTSTATUS
NTAPI
LdrpFindDllActivationContext(PLDR_DATA_TABLE_ENTRY LdrEntry);

NTSTATUS
NTAPI
LdrpPrepareImportAddressTableForSnap(PLDRP_LOAD_CONTEXT LoadContext);

NTSTATUS
NTAPI
LdrpNotifyLoadOfGraph(PLDR_DDAG_NODE Node);

NTSTATUS
NTAPI
LdrpSendPostSnapNotifications(PLDR_DDAG_NODE Node);

void
NTAPI
LdrpHandlePendingModuleReplaced(PLDRP_LOAD_CONTEXT LoadContext);

void
FASTCALL
LdrpMergeNodes(PLDR_DDAG_NODE Root, PSINGLE_LIST_ENTRY* CondenseLink);

void
FASTCALL
LdrpDestroyNode(PLDR_DDAG_NODE Node);

