#include <cstddef>
#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32_NO_STATUS
#define WIN32_NO_STATUS
#endif

#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winioctl.h>
    //#define _EXCEPTION_REGISTRATION_RECORD _EXCEPTION_REGISTRATION_RECORD2
    //#define EXCEPTION_REGISTRATION_RECORD EXCEPTION_REGISTRATION_RECORD2
    //#define PEXCEPTION_REGISTRATION_RECORD PEXCEPTION_REGISTRATION_RECORD2
#define _NTLDR_H
#define _NTDBG_H
#define _NTTP_H
#define _NTWOW64_H
#define _NTPFAPI_H
#define _NTLPCAPI_H
#define _NTOBAPI_H
#define _NTZWAPI_H

    typedef double DOUBLE;
    typedef GUID* PGUID;

    typedef struct _RTL_PROCESS_MODULE_INFORMATION
    {
        HANDLE Section;
        PVOID MappedBase;
        PVOID ImageBase;
        ULONG ImageSize;
        ULONG Flags;
        USHORT LoadOrderIndex;
        USHORT InitOrderIndex;
        USHORT LoadCount;
        USHORT OffsetToFileName;
        UCHAR FullPathName[256];
    } RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

    typedef struct _RTL_PROCESS_MODULE_INFORMATION_EX
    {
        USHORT NextOffset;
        RTL_PROCESS_MODULE_INFORMATION BaseInfo;
        ULONG ImageChecksum;
        ULONG TimeDateStamp;
        PVOID DefaultBase;
    } RTL_PROCESS_MODULE_INFORMATION_EX, * PRTL_PROCESS_MODULE_INFORMATION_EX;

#include <phnt.h>
#include "ldrdefs.h"
#include "ldrdata.h"

#ifdef __cplusplus
}
#endif

#include "fmt/format.h"

#if defined(_WIN64)
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, InLoadOrderLinks) == 0x000);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks) == 0x010);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks) == 0x020);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, DllBase) == 0x030);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, EntryPoint) == 0x038);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, SizeOfImage) == 0x040);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, FullDllName) == 0x048);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, BaseDllName) == 0x058);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, Flags) == 0x068);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, ObsoleteLoadCount) == 0x06C);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, TlsIndex) == 0x06E);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, HashLinks) == 0x070);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, TimeDateStamp) == 0x080);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, EntryPointActivationContext) == 0x088);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, DdagNode) == 0x098);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, NodeModuleLink) == 0x0A0);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, LoadContext) == 0x0B0);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, OriginalBase) == 0x0F8);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, LoadTime) == 0x100);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, BaseNameHashValue) == 0x108);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, LoadReason) == 0x10C);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, ImplicitPathOptions) == 0x110);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, ReferenceCount) == 0x114);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, DependentLoadFlags) == 0x118);
static_assert(offsetof(LDR_DDAG_NODE, Modules) == 0x000);
static_assert(offsetof(LDR_DDAG_NODE, ServiceTagList) == 0x010);
static_assert(offsetof(LDR_DDAG_NODE, LoadCount) == 0x018);
static_assert(offsetof(LDR_DDAG_NODE, LoadWhileUnloadingCount) == 0x01C);
static_assert(offsetof(LDR_DDAG_NODE, LowestLink) == 0x020);
static_assert(offsetof(LDR_DDAG_NODE, Dependencies) == 0x028);
static_assert(offsetof(LDR_DDAG_NODE, IncomingDependencies) == 0x030);
static_assert(offsetof(LDR_DDAG_NODE, State) == 0x038);
static_assert(offsetof(LDR_DDAG_NODE, CondenseLink) == 0x040);
static_assert(offsetof(LDR_DDAG_NODE, PreorderNumber) == 0x048);
static_assert(offsetof(LDRP_LOAD_CONTEXT, DllName) == 0x000);
static_assert(offsetof(LDRP_LOAD_CONTEXT, PathSearchContext) == 0x010);
static_assert(offsetof(LDRP_LOAD_CONTEXT, Flags) == 0x020);
static_assert(offsetof(LDRP_LOAD_CONTEXT, StatusResponse) == 0x028);
static_assert(offsetof(LDRP_LOAD_CONTEXT, ParentEntry) == 0x030);
static_assert(offsetof(LDRP_LOAD_CONTEXT, Entry) == 0x038);
static_assert(offsetof(LDRP_LOAD_CONTEXT, PendingDependencyEntry) == 0x050);
static_assert(offsetof(LDRP_LOAD_CONTEXT, ImportEntries) == 0x058);
static_assert(offsetof(LDRP_LOAD_CONTEXT, ImportEntriesCount) == 0x060);
static_assert(offsetof(LDRP_LOAD_CONTEXT, CountOfDependenciesPendingMap) == 0x064);
static_assert(offsetof(LDRP_LOAD_CONTEXT, BaseAddressOfIAT) == 0x068);
static_assert(offsetof(LDRP_LOAD_CONTEXT, SizeOfIAT) == 0x070);
static_assert(offsetof(LDRP_LOAD_CONTEXT, ImportDescriptor) == 0x080);
static_assert(offsetof(LDRP_LOAD_CONTEXT, OriginalIATProtection) == 0x088);
static_assert(offsetof(LDRP_LOAD_CONTEXT, GuardCFCheckFunctionPointer) == 0x090);
static_assert(offsetof(LDRP_LOAD_CONTEXT, GuardCFCheckFunctionPointerThunk) == 0x098);
static_assert(offsetof(LDRP_LOAD_CONTEXT, DllNameStorage) == 0x0C0);
#elif defined(_M_IX86)
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, InLoadOrderLinks) == 0x000);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks) == 0x008);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks) == 0x010);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, DllBase) == 0x018);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, EntryPoint) == 0x01C);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, SizeOfImage) == 0x020);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, FullDllName) == 0x024);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, BaseDllName) == 0x02C);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, Flags) == 0x034);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, ObsoleteLoadCount) == 0x038);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, TlsIndex) == 0x03A);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, HashLinks) == 0x03C);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, TimeDateStamp) == 0x044);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, EntryPointActivationContext) == 0x048);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, Lock) == 0x04C);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, DdagNode) == 0x050);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, NodeModuleLink) == 0x054);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, LoadContext) == 0x05C);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, ParentDllBase) == 0x060);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, SwitchBackContext) == 0x064);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, BaseAddressIndexNode) == 0x068);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, MappingInfoIndexNode) == 0x074);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, OriginalBase) == 0x080);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, LoadTime) == 0x088);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, BaseNameHashValue) == 0x090);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, LoadReason) == 0x094);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, ImplicitPathOptions) == 0x098);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, ReferenceCount) == 0x09C);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, DependentLoadFlags) == 0x0A0);
static_assert(offsetof(LDR_DATA_TABLE_ENTRY, SigningLevel) == 0x0A4);
static_assert(offsetof(LDR_DDAG_NODE, Modules) == 0x000);
static_assert(offsetof(LDR_DDAG_NODE, ServiceTagList) == 0x008);
static_assert(offsetof(LDR_DDAG_NODE, LoadCount) == 0x00C);
static_assert(offsetof(LDR_DDAG_NODE, LoadWhileUnloadingCount) == 0x010);
static_assert(offsetof(LDR_DDAG_NODE, LowestLink) == 0x014);
static_assert(offsetof(LDR_DDAG_NODE, Dependencies) == 0x018);
static_assert(offsetof(LDR_DDAG_NODE, IncomingDependencies) == 0x01C);
static_assert(offsetof(LDR_DDAG_NODE, State) == 0x020);
static_assert(offsetof(LDR_DDAG_NODE, CondenseLink) == 0x024);
static_assert(offsetof(LDR_DDAG_NODE, PreorderNumber) == 0x028);
static_assert(offsetof(LDRP_LOAD_CONTEXT, DllName) == 0x000);
#else
#error
#endif

#if defined(_M_IX86)
#  define ARCH_A          "x86"
#  define ARCH_W         L"x86"
#elif defined(_M_AMD64)
#  define ARCH_A          "x64"
#  define ARCH_W         L"x64"
#else
#  error Unknown architecture
#endif

_Use_decl_annotations_
[[nodiscard]]
_VCRT_ALLOCATOR
void* __CRTDECL operator new(std::size_t sz) {
    void* ptr = RtlAllocateHeap(GetProcessHeap(), 0, sz);
    if (ptr)
        return ptr;
    else
        throw std::bad_alloc{};
}

_Use_decl_annotations_
[[nodiscard]]
_VCRT_ALLOCATOR
void* __CRTDECL operator new(std::size_t sz, const std::nothrow_t&) noexcept {
    return RtlAllocateHeap(GetProcessHeap(), 0, sz);
}

void __CRTDECL operator delete(void* ptr) noexcept
{
    RtlFreeHeap(GetProcessHeap(), 0, ptr);
}

inline fmt::wstring_view str(const UNICODE_STRING* p) {
    if (!p)
        return L"<nullptr>";
    if (!p->Buffer)
        return L"<NULL>";
    if (!p->Length)
        return L"<empty>";

    return fmt::wstring_view(p->Buffer, p->Length / sizeof(WCHAR));
}

inline std::wstring str(const LDRP_UNICODE_STRING_BUNDLE* p) {
    const auto view = str(&p->String);
    const std::wstring result{ view.data(), view.size() };

    if (!p || !p->String.Buffer)
        return result;

    if (p->String.Buffer == p->StaticBuffer)
        return L"[S] " + result;

    return L"[D] " + result;
}

inline fmt::wstring_view str(const LDR_DATA_TABLE_ENTRY* p) {
    if (!p)
        return L"<nullptr>";
    return str(&p->BaseDllName);
}

inline std::wstring str(BOOLEAN p) {
    return p ? L"T" : L"F";
}

template <class T>
struct Mallocator {
    typedef T value_type;
    Mallocator() = default;
    template <class U> constexpr Mallocator(const Mallocator<U>&) noexcept {}
    [[nodiscard]] T* allocate(std::size_t n) {
        if (n > std::size_t(-1) / sizeof(T)) throw std::bad_alloc();
        if (auto p = static_cast<T*>(new (std::nothrow) T[n])) return p;
        throw std::bad_alloc();
    }
    void deallocate(T* p, std::size_t) noexcept { delete[] p; }
};

template <class T, class U>
bool operator==(const Mallocator<T>&, const Mallocator<U>&) { return true; }
template <class T, class U>
bool operator!=(const Mallocator<T>&, const Mallocator<U>&) { return false; }

using custom_memory_buffer =
fmt::basic_memory_buffer<wchar_t, fmt::inline_buffer_size, Mallocator<wchar_t>>;

using custom_string =
std::basic_string<wchar_t, std::char_traits<wchar_t>, Mallocator<wchar_t>>;

Mallocator<wchar_t> Malloc;

custom_string vformat(fmt::wstring_view format_str,
    fmt::wformat_args args) {
    custom_memory_buffer buf(Malloc);
    fmt::vformat_to(buf, format_str, args);
    return custom_string(buf.data(), buf.size(), Malloc);
}

template <typename... Args>
inline
void dbgprint(fmt::wstring_view format_str, Args&& ... args)
{
    auto str = vformat(
        format_str,
        fmt::internal::make_args_checked<Args...>(format_str, args...)
    );

    OutputDebugStringW(str.c_str());
}

// Include Detours.

#include "detours.h"

HANDLE NtdllHandle;

UINT_PTR TargetOffsets[HookTargetsCount];

// Hooking functions and prototypes.

EXTERN_C
void
FASTCALL
HookLdrpCondenseGraphRecurse(LDR_DDAG_NODE* Node, ULONG32* PreorderNumberStorage, PSINGLE_LIST_ENTRY* CurrentRootCondenseLink);

decltype(LdrpAllocateModuleEntry)* OrigLdrpAllocateModuleEntry = nullptr;
decltype(LdrpMergeNodes)* OrigLdrpMergeNodes = nullptr;
decltype(LdrpDestroyNode)* OrigLdrpDestroyNode = nullptr;
decltype(LdrpMapDllWithSectionHandle)* OrigLdrpMapDllWithSectionHandle = nullptr;
decltype(LdrpFreeLoadContext)* OrigLdrpFreeLoadContext = nullptr;
decltype(LdrpUnmapModule)* OrigLdrpUnmapModule = nullptr;
decltype(HookLdrpCondenseGraphRecurse)* OrigLdrpCondenseGraphRecurse = nullptr;
decltype(LdrpPreprocessDllName)* OrigLdrpPreprocessDllName = nullptr;
decltype(LdrpApplyFileNameRedirection)* OrigLdrpApplyFileNameRedirection = nullptr;

EXTERN_C decltype(LdrpAllocateModuleEntry) HookLdrpAllocateModuleEntry;
EXTERN_C decltype(LdrpMergeNodes) HookLdrpMergeNodes;
EXTERN_C decltype(LdrpDestroyNode) HookLdrpDestroyNode;
EXTERN_C decltype(LdrpMapDllWithSectionHandle) HookLdrpMapDllWithSectionHandle;
EXTERN_C decltype(LdrpUnmapModule) HookLdrpUnmapModule;
//EXTERN_C decltype(LdrpCondenseGraphRecurse) HookLdrpCondenseGraphRecurse;
EXTERN_C decltype(LdrpPreprocessDllName) HookLdrpPreprocessDllName;
EXTERN_C decltype(LdrpApplyFileNameRedirection) HookLdrpApplyFileNameRedirection;

EXTERN_C
PLDR_DATA_TABLE_ENTRY
NTAPI
HookLdrpAllocateModuleEntry(IN PLDRP_LOAD_CONTEXT LoadContext OPTIONAL)
{
    PLDR_DATA_TABLE_ENTRY Entry = OrigLdrpAllocateModuleEntry(LoadContext);

    dbgprint(
        fmt(L"[inj]: HookLdrpAllocateModuleEntry({}) -> {}"),
        str(&LoadContext->DllName),
        (PVOID)Entry
    );

    return Entry;
}

EXTERN_C
void
FASTCALL
HookLdrpMergeNodes(PLDR_DDAG_NODE Root, PSINGLE_LIST_ENTRY* CondenseLink)
{
    PLDR_DATA_TABLE_ENTRY RootEntry = CONTAINING_RECORD(Root->Modules.Flink, LDR_DATA_TABLE_ENTRY, NodeModuleLink);
    dbgprint(
        fmt(L"[inj]: HookLdrpMergeNodes({}, ...)"),
        str(RootEntry)
    );

    PSINGLE_LIST_ENTRY CondenseCurrentLink = *CondenseLink;
    UINT_PTR Count = 0;

    do {
        PLDR_DDAG_NODE CurrentNode = CONTAINING_RECORD(CondenseCurrentLink, LDR_DDAG_NODE, CondenseLink);
        PLDR_DATA_TABLE_ENTRY CurrentEntry = CONTAINING_RECORD(CurrentNode->Modules.Flink, LDR_DATA_TABLE_ENTRY, NodeModuleLink);

        dbgprint(
            fmt(L"[inj]: #{}: {}"),
            Count,
            str(CurrentEntry)
        );

        Count++;
        CondenseCurrentLink = CondenseCurrentLink->Next;
    } while (CondenseCurrentLink);

    OrigLdrpMergeNodes(Root, CondenseLink);

    dbgprint(
        fmt(L"[inj]: LdrpMergeNodes EXIT")
    );
}

EXTERN_C
void
FASTCALL
HookLdrpDestroyNode(PLDR_DDAG_NODE Node)
{
    dbgprint(
        fmt(L"[inj]: HookLdrpDestroyNode({})"),
        (PVOID)Node
    );

    OrigLdrpDestroyNode(Node);
}

EXTERN_C
NTSTATUS
FASTCALL
HookLdrpMapDllWithSectionHandle(PLDRP_LOAD_CONTEXT LoadContext, HANDLE SectionHandle)
{
    dbgprint(
        fmt(L"[inj]: HookLdrpMapDllWithSectionHandle({}, ...)"),
        (PVOID)LoadContext
    );

    return OrigLdrpMapDllWithSectionHandle(LoadContext, SectionHandle);
}

EXTERN_C
void
FASTCALL
HookLdrpFreeLoadContext(PLDRP_LOAD_CONTEXT LoadContext)
{
    dbgprint(
        fmt(L"[inj]: HookLdrpFreeLoadContext({})"),
        (PVOID)LoadContext
    );

    OrigLdrpFreeLoadContext(LoadContext);
}

EXTERN_C
NTSTATUS
FASTCALL
HookLdrpUnmapModule(IN PLDR_DATA_TABLE_ENTRY LdrEntry)
{
    dbgprint(
        fmt(L"[inj]: HookLdrpUnmapModule({})"),
        (PVOID)LdrEntry
    );

    return OrigLdrpUnmapModule(LdrEntry);
}

EXTERN_C
void
FASTCALL
HookLdrpCondenseGraphRecurse(LDR_DDAG_NODE* Node, ULONG32* PreorderNumberStorage, PSINGLE_LIST_ENTRY* CurrentRootCondenseLink)
{
    dbgprint(
        fmt(L"[inj]: HookLdrpCondenseGraphRecurse({}, {}, {}) ENTER"),
        (PVOID)Node,
        *PreorderNumberStorage,
        (PVOID)CurrentRootCondenseLink
    );

    OrigLdrpCondenseGraphRecurse(Node, PreorderNumberStorage, CurrentRootCondenseLink);

    dbgprint(
        fmt(L"[inj]: HookLdrpCondenseGraphRecurse({}, {}, {}) EXIT"),
        (PVOID)Node,
        *PreorderNumberStorage,
        (PVOID)CurrentRootCondenseLink
    );
}

EXTERN_C
NTSTATUS
FASTCALL
HookLdrpPreprocessDllName(IN PUNICODE_STRING DllName,
    IN OUT PLDRP_UNICODE_STRING_BUNDLE OutputDllName,
    IN PLDR_DATA_TABLE_ENTRY ParentEntry OPTIONAL,
    OUT PLDRP_LOAD_CONTEXT_FLAGS LoadContextFlags)
{
    dbgprint(
        fmt(L"[inj]: HookLdrpPreprocessDllName({}, {}, {}, {:#b}) ENTER"),
        str(DllName),
        str(OutputDllName),
        str(ParentEntry),
        LoadContextFlags->Flags
    );

    auto res = OrigLdrpPreprocessDllName(DllName, OutputDllName, ParentEntry, LoadContextFlags);

    dbgprint(
        fmt(L"[inj]: HookLdrpPreprocessDllName({}, {}, {}, {:#b}) EXIT [{:X}]"),
        str(DllName),
        str(OutputDllName),
        str(ParentEntry),
        LoadContextFlags->Flags,
        res
    );

    return res;
}

EXTERN_C
NTSTATUS
FASTCALL
HookLdrpApplyFileNameRedirection(IN PLDR_DATA_TABLE_ENTRY ParentEntry,
    IN PUNICODE_STRING DllName,
    IN PVOID Unused,
    IN OUT PLDRP_UNICODE_STRING_BUNDLE RedirectedDllName,
    OUT PBOOLEAN RedirectedSxS)
{
    dbgprint(
        fmt(L"[inj]: HookLdrpApplyFileNameRedirection({}, {}, {}, {}, {}) ENTER"),
        str(ParentEntry),
        str(DllName),
        Unused,
        str(RedirectedDllName),
        (PVOID)RedirectedSxS
    );

    auto res = OrigLdrpApplyFileNameRedirection(ParentEntry, DllName, Unused, RedirectedDllName, RedirectedSxS);

    dbgprint(
        fmt(L"[inj]: HookLdrpApplyFileNameRedirection({}, {}, {}, {}, {}) EXIT [{:X}]"),
        str(ParentEntry),
        str(DllName),
        Unused,
        str(RedirectedDllName),
        str(*RedirectedSxS),
        res
    );

    return res;
}

NTSTATUS
NTAPI
EnableDetours()
{
    assert(DetourTransactionBegin() == NO_ERROR);
    assert(DetourUpdateThread(GetCurrentThread()) == NO_ERROR);

    PVOID Target;

    {
        Target = PTR_ADD_OFFSET(NtdllHandle, TargetOffsets[HookTargetLdrpAllocateModuleEntry]);
        OrigLdrpAllocateModuleEntry = static_cast<decltype(OrigLdrpAllocateModuleEntry)>(Target);
        assert(DetourAttach((PVOID*)&OrigLdrpAllocateModuleEntry, HookLdrpAllocateModuleEntry) == NO_ERROR);
    }
    {
        Target = PTR_ADD_OFFSET(NtdllHandle, TargetOffsets[HookTargetLdrpMergeNodes]);
        OrigLdrpMergeNodes = static_cast<decltype(OrigLdrpMergeNodes)>(Target);
        assert(DetourAttach((PVOID*)&OrigLdrpMergeNodes, HookLdrpMergeNodes) == NO_ERROR);
    }
    {
        Target = PTR_ADD_OFFSET(NtdllHandle, TargetOffsets[HookTargetLdrpDestroyNode]);
        OrigLdrpDestroyNode = static_cast<decltype(OrigLdrpDestroyNode)>(Target);
        assert(DetourAttach((PVOID*)&OrigLdrpDestroyNode, HookLdrpDestroyNode) == NO_ERROR);
    }
    {
        Target = PTR_ADD_OFFSET(NtdllHandle, TargetOffsets[HookTargetLdrpMapDllWithSectionHandle]);
        OrigLdrpMapDllWithSectionHandle = static_cast<decltype(OrigLdrpMapDllWithSectionHandle)>(Target);
        assert(DetourAttach((PVOID*)&OrigLdrpMapDllWithSectionHandle, HookLdrpMapDllWithSectionHandle) == NO_ERROR);
    }
    {
        Target = PTR_ADD_OFFSET(NtdllHandle, TargetOffsets[HookTargetLdrpFreeLoadContext]);
        OrigLdrpFreeLoadContext = static_cast<decltype(OrigLdrpFreeLoadContext)>(Target);
        assert(DetourAttach((PVOID*)&OrigLdrpFreeLoadContext, HookLdrpFreeLoadContext) == NO_ERROR);
    }
    {
        Target = PTR_ADD_OFFSET(NtdllHandle, TargetOffsets[HookTargetLdrpUnmapModule]);
        OrigLdrpUnmapModule = static_cast<decltype(OrigLdrpUnmapModule)>(Target);
        assert(DetourAttach((PVOID*)&OrigLdrpUnmapModule, HookLdrpUnmapModule) == NO_ERROR);
    }
    {
        Target = PTR_ADD_OFFSET(NtdllHandle, TargetOffsets[HookTargetLdrpCondenseGraphRecurse]);
        OrigLdrpCondenseGraphRecurse = static_cast<decltype(OrigLdrpCondenseGraphRecurse)>(Target);
        assert(DetourAttach((PVOID*)&OrigLdrpCondenseGraphRecurse, HookLdrpCondenseGraphRecurse) == NO_ERROR);
    }
    {
        Target = PTR_ADD_OFFSET(NtdllHandle, TargetOffsets[HookTargetLdrpPreprocessDllName]);
        OrigLdrpPreprocessDllName = static_cast<decltype(OrigLdrpPreprocessDllName)>(Target);
        assert(DetourAttach((PVOID*)&OrigLdrpPreprocessDllName, HookLdrpPreprocessDllName) == NO_ERROR);
    }
    {
        Target = PTR_ADD_OFFSET(NtdllHandle, TargetOffsets[HookTargetLdrpApplyFileNameRedirection]);
        OrigLdrpApplyFileNameRedirection = static_cast<decltype(OrigLdrpApplyFileNameRedirection)>(Target);
        assert(DetourAttach((PVOID*)&OrigLdrpApplyFileNameRedirection, HookLdrpApplyFileNameRedirection) == NO_ERROR);
    }

    assert(DetourTransactionCommit() == NO_ERROR);

    return STATUS_SUCCESS;
}

NTSTATUS
NTAPI
DisableDetours()
{
    assert(DetourTransactionBegin() == NO_ERROR);
    assert(DetourUpdateThread(GetCurrentThread()) == NO_ERROR);

    assert(DetourDetach((PVOID*)&OrigLdrpAllocateModuleEntry, HookLdrpAllocateModuleEntry) == NO_ERROR);
    assert(DetourDetach((PVOID*)&OrigLdrpMergeNodes, HookLdrpMergeNodes) == NO_ERROR);
    assert(DetourDetach((PVOID*)&OrigLdrpDestroyNode, HookLdrpDestroyNode) == NO_ERROR);
    assert(DetourDetach((PVOID*)&OrigLdrpMapDllWithSectionHandle, HookLdrpMapDllWithSectionHandle) == NO_ERROR);
    assert(DetourDetach((PVOID*)&OrigLdrpFreeLoadContext, HookLdrpFreeLoadContext) == NO_ERROR);
    assert(DetourDetach((PVOID*)&OrigLdrpUnmapModule, HookLdrpUnmapModule) == NO_ERROR);
    assert(DetourDetach((PVOID*)&OrigLdrpCondenseGraphRecurse, HookLdrpCondenseGraphRecurse) == NO_ERROR);
    assert(DetourDetach((PVOID*)&OrigLdrpPreprocessDllName, HookLdrpPreprocessDllName) == NO_ERROR);
    assert(DetourDetach((PVOID*)&OrigLdrpApplyFileNameRedirection, HookLdrpApplyFileNameRedirection) == NO_ERROR);

    assert(DetourTransactionCommit() == NO_ERROR);

    return STATUS_SUCCESS;
}

NTSTATUS
NTAPI
OnProcessAttach(_In_ PVOID ModuleHandle)
{
    UNICODE_STRING NtdllPath;
    RtlInitUnicodeString(&NtdllPath, (PWSTR)L"ntdll.dll");

    LdrGetDllHandle(NULL, 0, &NtdllPath, &NtdllHandle);

    // Make us unloadable (by FreeLibrary calls).

    LdrAddRefDll(LDR_ADDREF_DLL_PIN, ModuleHandle);

    PPEB Peb = NtCurrentPeb();

    dbgprint(
        fmt(L"[inj]: Arch: {}, NTDLL: {}, me: {}, CommandLine: {}"),
        ARCH_W,
        NtdllHandle,
        ModuleHandle,
        Peb->ProcessParameters->CommandLine.Buffer
    );

    dbgprint(
        fmt(L"[inj]|={}"),
        (UINT_PTR)TargetOffsets
    );

    // Hook all functions.

    return EnableDetours();
}

NTSTATUS
NTAPI
OnProcessDetach()
{
    // Unhook all functions.

    return DisableDetours();
}

EXTERN_C
BOOL
NTAPI
NtDllMain(
    _In_ HANDLE ModuleHandle,
    _In_ ULONG Reason,
    _In_ LPVOID Reserved)
{
    switch (Reason)
    {
    case DLL_PROCESS_ATTACH:
        OnProcessAttach(ModuleHandle);
        break;

    case DLL_PROCESS_DETACH:
        OnProcessDetach();
        break;

    case DLL_THREAD_ATTACH:

        break;

    case DLL_THREAD_DETACH:

        break;
    }

    return TRUE;
}
