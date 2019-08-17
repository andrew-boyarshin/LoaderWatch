#include "common.h"

PPH_SYMBOL_PROVIDER SymbolProvider = NULL;

typedef struct _THREAD_STACK_ITEM
{
    PH_THREAD_STACK_FRAME StackFrame;
    ULONG Index;
    PPH_STRING Symbol;
} THREAD_STACK_ITEM, *PTHREAD_STACK_ITEM;

typedef void (__stdcall* PhpWalkThreadStackCallback_CALLBACK)(PTHREAD_STACK_ITEM item);

typedef struct _PH_THREAD_STACK_CONTEXT
{
    HANDLE ProcessId;
    HANDLE ThreadId;
    HANDLE ThreadHandle;

    BOOL StopWalk;

    ULONG Count;

    NTSTATUS WalkStatus;

    PhpWalkThreadStackCallback_CALLBACK Callback;
    PH_CALLBACK_REGISTRATION SymbolProviderEventRegistration;
} PH_THREAD_STACK_CONTEXT, *PPH_THREAD_STACK_CONTEXT;

typedef enum _PH_STACK_TREE_COLUMN_ITEM_NAME
{
    PH_STACK_TREE_COLUMN_INDEX,
    PH_STACK_TREE_COLUMN_SYMBOL,
    PH_STACK_TREE_COLUMN_STACKADDRESS,
    PH_STACK_TREE_COLUMN_FRAMEADDRESS,
    PH_STACK_TREE_COLUMN_PARAMETER1,
    PH_STACK_TREE_COLUMN_PARAMETER2,
    PH_STACK_TREE_COLUMN_PARAMETER3,
    PH_STACK_TREE_COLUMN_PARAMETER4,
    PH_STACK_TREE_COLUMN_CONTROLADDRESS,
    PH_STACK_TREE_COLUMN_RETURNADDRESS,
    TREE_COLUMN_ITEM_MAXIMUM
} PH_STACK_TREE_COLUMN_ITEM_NAME;

typedef struct _PH_STACK_TREE_ROOT_NODE
{
    PH_THREAD_STACK_FRAME StackFrame;

    ULONG Index;
    PPH_STRING TooltipText;
    PPH_STRING IndexString;
    PPH_STRING SymbolString;
    WCHAR StackAddressString[PH_PTR_STR_LEN_1];
    WCHAR FrameAddressString[PH_PTR_STR_LEN_1];
    WCHAR Parameter1String[PH_PTR_STR_LEN_1];
    WCHAR Parameter2String[PH_PTR_STR_LEN_1];
    WCHAR Parameter3String[PH_PTR_STR_LEN_1];
    WCHAR Parameter4String[PH_PTR_STR_LEN_1];
    WCHAR PcAddressString[PH_PTR_STR_LEN_1];
    WCHAR ReturnAddressString[PH_PTR_STR_LEN_1];

    PH_STRINGREF TextCache[TREE_COLUMN_ITEM_MAXIMUM];
} PH_STACK_TREE_ROOT_NODE, *PPH_STACK_TREE_ROOT_NODE;

BOOLEAN
NTAPI
PhpWalkThreadStackCallback(
    _In_ PPH_THREAD_STACK_FRAME StackFrame,
    _In_opt_ PVOID Context
)
{
    PPH_THREAD_STACK_CONTEXT threadStackContext = (PPH_THREAD_STACK_CONTEXT)Context;
    PPH_STRING symbol;
    PTHREAD_STACK_ITEM item;

    if (threadStackContext->StopWalk)
        return FALSE;

    symbol = PhGetSymbolFromAddress(
        SymbolProvider,
        (ULONG64)StackFrame->PcAddress,
        NULL,
        NULL,
        NULL,
        NULL
    );

    if (symbol &&
        (StackFrame->Flags & PH_THREAD_STACK_FRAME_I386) &&
        !(StackFrame->Flags & PH_THREAD_STACK_FRAME_FPO_DATA_PRESENT))
    {
        PhMoveReference(&symbol, PhConcatStrings2(symbol->Buffer, L" (No unwind info)"));
    }

    item = PhAllocateZero(sizeof(THREAD_STACK_ITEM));
    item->StackFrame = *StackFrame;
    item->Index = threadStackContext->Count++;
    item->Symbol = symbol;

    threadStackContext->Callback(item);

    return TRUE;
}

VOID
NTAPI
PhpSymbolProviderEventCallbackHandler(
    _In_opt_ PVOID Parameter,
    _In_opt_ PVOID Context
)
{
    PPH_SYMBOL_EVENT_DATA event = Parameter;
    PPH_THREAD_STACK_CONTEXT context = Context;
    PPH_STRING statusMessage = NULL;

    switch (event->ActionCode)
    {
        case CBA_DEFERRED_SYMBOL_LOAD_START:
        case CBA_DEFERRED_SYMBOL_LOAD_COMPLETE:
        case CBA_DEFERRED_SYMBOL_LOAD_FAILURE:
        case CBA_SYMBOLS_UNLOADED:
        {
            PIMAGEHLP_DEFERRED_SYMBOL_LOADW64 callbackData = (PIMAGEHLP_DEFERRED_SYMBOL_LOADW64)event->EventData;
            PPH_STRING fileName = NULL;

            if (callbackData->FileName[0] != UNICODE_NULL)
            {
                fileName = PhCreateString(callbackData->FileName);
                PhMoveReference(&fileName, PhGetBaseName(fileName));
            }

            switch (event->ActionCode)
            {
                case CBA_DEFERRED_SYMBOL_LOAD_START:
                    statusMessage = PhFormatString(L"Loading symbols from %s...", PhGetStringOrEmpty(fileName));
                    break;
                case CBA_DEFERRED_SYMBOL_LOAD_COMPLETE:
                    statusMessage = PhFormatString(L"Loaded symbols from %s...", PhGetStringOrEmpty(fileName));
                    break;
                case CBA_DEFERRED_SYMBOL_LOAD_FAILURE:
                    statusMessage = PhFormatString(L"Failed to load %s...", PhGetStringOrEmpty(fileName));
                    break;
                case CBA_SYMBOLS_UNLOADED:
                    statusMessage = PhFormatString(L"Unloading %s...", PhGetStringOrEmpty(fileName));
                    break;
            }

            if (fileName)
                PhDereferenceObject(fileName);
        }
            break;
        case CBA_READ_MEMORY:
        {
            //PIMAGEHLP_CBA_READ_MEMORY callbackEvent = (PIMAGEHLP_CBA_READ_MEMORY)event->EventData;
            //statusMessage = PhFormatString(L"Reading %lu bytes of memory from 0x%I64X...", callbackEvent->bytes, callbackEvent->addr);
        }
        break;
        case CBA_EVENT:
        {
            PIMAGEHLP_CBA_EVENTW callbackEvent = (PIMAGEHLP_CBA_EVENTW)event->EventData;
            statusMessage = PhFormatString(L"%s", callbackEvent->desc);
        }
        break;
        case CBA_DEBUG_INFO:
        {
            statusMessage = PhFormatString(L"%p", event->EventData);
        }
        break;
        case CBA_ENGINE_PRESENT:
        case CBA_DEFERRED_SYMBOL_LOAD_PARTIAL:
        case CBA_DEFERRED_SYMBOL_LOAD_CANCEL:
        default:
        {
            //statusMessage = PhFormatString(L"Unknown: %lu", event->ActionCode);
        }
            break;
    }

    if (statusMessage)
    {
        printf_s("%S\r\n", statusMessage->Buffer);
    }
}

PHAPPAPI
NTSTATUS
NTAPI
PhpRefreshThreadStackThreadStart(
    _In_ PPH_THREAD_STACK_CONTEXT threadStackContext
)
{
    PH_AUTO_POOL autoPool;
    NTSTATUS status;
    CLIENT_ID clientId;

    PhInitializeAutoPool(&autoPool);

    clientId.UniqueProcess = threadStackContext->ProcessId;
    clientId.UniqueThread = threadStackContext->ThreadId;

    PhRegisterCallback(
        &PhSymbolEventCallback,
        PhpSymbolProviderEventCallbackHandler,
        threadStackContext,
        &threadStackContext->SymbolProviderEventRegistration
    );

    status = PhWalkThreadStack(
        threadStackContext->ThreadHandle,
        SymbolProvider->ProcessHandle,
        &clientId,
        SymbolProvider,
        PH_WALK_I386_STACK | PH_WALK_AMD64_STACK | PH_WALK_KERNEL_STACK | 0x20,
        PhpWalkThreadStackCallback,
        threadStackContext
    );

    if (threadStackContext->Count != 0)
        status = STATUS_SUCCESS;

    threadStackContext->WalkStatus = status;

    PhUnregisterCallback(
        &PhSymbolEventCallback,
        &threadStackContext->SymbolProviderEventRegistration
    );

    PhDeleteAutoPool(&autoPool);

    return STATUS_SUCCESS;
}

PHAPPAPI
void
NTAPI
PhCreateThreadProvider(
    _In_ HANDLE ProcessId
)
{
    SymbolProvider = PhCreateSymbolProvider(ProcessId);
}

PHAPPAPI
BOOL
NTAPI
PhLoadModuleDefaultSymbolProvider(
    _In_ PLDR_DATA_TABLE_ENTRY Module,
    _In_ PWSTR FullDllName
)
{
    if (!SymbolProvider)
        return FALSE;

    PPH_STRING OriginalFileName = PhCreateString(FullDllName);
    PPH_STRING FileName = PhGetFileName(OriginalFileName);

    BOOLEAN Res = PhLoadModuleSymbolProvider(
        SymbolProvider,
        FileName->Buffer,
        (ULONG64)Module->DllBase,
        Module->SizeOfImage
    );

    PhDereferenceObject(FileName);
    PhDereferenceObject(OriginalFileName);

    return (BOOL)Res;
}

PHAPPAPI
NTSTATUS
NTAPI
PhOpenThreadExport(
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ HANDLE ThreadId
)
{
    return PhOpenThread(ThreadHandle, DesiredAccess, ThreadId);
}

PHAPPAPI
void
NTAPI
SetSearchPathSymbolProvider(
    _In_ PWSTR Path
)
{
    PhSetSearchPathSymbolProvider(SymbolProvider, Path);
}

typedef void(__stdcall* LookupSymbolOffsetCallback_CALLBACK)(PPH_SYMBOL_INFO item);

typedef struct _PH_LOOKUP_SYMBOL_OFFSET_CONTEXT
{
    LookupSymbolOffsetCallback_CALLBACK UserCallback;
} PH_LOOKUP_SYMBOL_OFFSET_CONTEXT, * PPH_LOOKUP_SYMBOL_OFFSET_CONTEXT;

BOOLEAN
NTAPI
LookupSymbolOffsetCallback(
    _In_ PPH_SYMBOL_INFO pSymInfo,
    _In_ ULONG SymbolSize,
    _In_ PVOID UserContext
)
{
    PPH_LOOKUP_SYMBOL_OFFSET_CONTEXT Context = (PPH_LOOKUP_SYMBOL_OFFSET_CONTEXT)UserContext;
    Context->UserCallback(pSymInfo);
    return TRUE;
}

PHAPPAPI
BOOL
NTAPI
LookupSymbolOffset(
    _In_ PCWSTR SymbolName,
    _In_ LookupSymbolOffsetCallback_CALLBACK Callback
)
{
    if (!SymbolProvider)
        return FALSE;

    PH_LOOKUP_SYMBOL_OFFSET_CONTEXT Context = { 0 };
    Context.UserCallback = Callback;

    return PhEnumerateSymbols(
        SymbolProvider,
        SymbolProvider->ProcessHandle,
        0,
        SymbolName,
        LookupSymbolOffsetCallback,
        &Context
    );
}
