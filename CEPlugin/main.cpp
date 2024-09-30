// example-c.cpp : Defines the entry point for the DLL application.
//

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#include <cstdio>
// #include "CheatEngine/cepluginsdk.h"


#include "Hooks/hooks.h"
#include "Memory/vad.h"


int PointerReassignmentPluginID = -1;
int MainMenuPluginID = -1;

ExportedFunctions Exported;

void __stdcall mainmenuplugin(void) {
    Exported.ShowMessage((char *) "Main menu plugin");
}

BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

BOOL __stdcall CEPlugin_GetVersion(PPluginVersion pv, int sizeofpluginversion) {
    pv->version = CESDK_VERSION;
    pv->pluginname = (char *) "Methicc's DMA plugin";
    return TRUE;
}

void __stdcall PointersReassigned(int reserved) {
    printf("Pointers got modified");
    /*auto open_process = Exported.OpenProcess;
    auto read_process_memory = Exported.ReadProcessMemory;
    auto write_process_memory = Exported.WriteProcessMemory;
    auto virtual_query = Exported.VirtualQueryEx;*/
}

BOOL __stdcall CEPlugin_InitializePlugin(PExportedFunctions ef, int pluginid) {
    MAINMENUPLUGIN_INIT init1;
    POINTERREASSIGNMENTPLUGIN_INIT init4;

    //open console
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
    freopen("conout$", "w+t", stdout);
    freopen("conin$", "r+t", stdin);
    printf("initializing\n");
    auto open_process = ef->OpenProcess;
    auto read_process_memory = ef->ReadProcessMemory;
    auto write_process_memory = ef->WriteProcessMemory;
    auto virtual_query = ef->VirtualQueryEx;
    auto create_tool_help32 = ef->CreateToolhelp32Snapshot;
    auto process_32_first = ef->Process32First;
    auto process_32_next = ef->Process32Next;
    auto module_32_first = ef->Module32First;
    auto module_32_next = ef->Module32Next;
    auto thread_32_first = ef->Thread32First;
    auto thread_32_next = ef->Thread32Next;
    auto GetAddressFromPointer = (uintptr_t) ef->GetAddressFromPointer;

    /*auto open_thread = ef->OpenThread;
    auto get_thread_context = ef->GetThreadContext;
    auto set_thread_context = ef->SetThreadContext;
    auto suspend_thread = ef->SuspendThread;
    auto resume_thread = ef->ResumeThread;*/

    printf("Initialize DMA in advance (Required for Set Thread Context)\n");
    memoryTools.init("");

    printf("Hooking Open Process 0x%p\n", open_process);
    *(uintptr_t *) (open_process) = (uintptr_t) &Hooks::hk_open_process;
    GetAddressFromPointer = (uintptr_t) &Hooks::GetAddressFromPointer;

    printf("Hooking Read 0x%p\n", read_process_memory);
    *(uintptr_t *) (read_process_memory) = (uintptr_t) &Hooks::hk_read;

    printf("Hooking Write 0x%p\n", write_process_memory);
    *(uintptr_t *) (write_process_memory) = (uintptr_t) &Hooks::hk_write;

    printf("Hooking Virtual Query 0x%p\n", virtual_query);
    *(uintptr_t *) (virtual_query) = (uintptr_t) &Hooks::hk_virtual_query;

    printf("Hooking CreateToolhelp32Snapshot 0x%p\n", create_tool_help32);
    *(uintptr_t *) (create_tool_help32) = (uintptr_t) &Hooks::hk_create_tool_help_32_snapshot;

    printf("Hooking Process32First 0x%p\n", process_32_first);
    *(uintptr_t *) (process_32_first) = (uintptr_t) &Hooks::hk_process_32_first;

    printf("Hooking Process32Next 0x%p\n", process_32_next);
    *(uintptr_t *) (process_32_next) = (uintptr_t) &Hooks::hk_process_32_next;

    printf("Hooking Module32First 0x%p\n", module_32_first);
    *(uintptr_t *) (module_32_first) = (uintptr_t) &Hooks::hk_module_32_first;

    printf("Hooking Module32Next 0x%p\n", module_32_next);
    *(uintptr_t *) (module_32_next) = (uintptr_t) &Hooks::hk_module_32_next;

    printf("Hooking Thread32First 0x%p\n", thread_32_first);
    *(uintptr_t *) (thread_32_first) = (uintptr_t) &Hooks::hk_thread_32_first;

    printf("Hooking Thread32Next 0x%p\n", thread_32_next);
    *(uintptr_t *) (thread_32_next) = (uintptr_t) &Hooks::hk_thread_32_next;

    //Check comment in Hooks.h for why this is commented out.
    /*
     printf("Hooking OpenThread 0x%p\n", open_thread);
    *(uintptr_t*)(open_thread) = (uintptr_t)&Hooks::hk_open_thread;

     printf("Hooking GetThreadContext 0x%p\n", get_thread_context);
    *(uintptr_t*)(get_thread_context) = (uintptr_t)&Hooks::hk_get_thread_context;

    printf("Hooking SetThreadContext 0x%p\n", set_thread_context);
    *(uintptr_t*)(set_thread_context) = (uintptr_t)&Hooks::hk_set_thread_context;

    printf("Hooking SuspendThread 0x%p\n", suspend_thread);
    *(uintptr_t*)(suspend_thread) = (uintptr_t)&Hooks::hk_suspend_thread;

    printf("Hooking ResumeThread 0x%p\n", resume_thread);
    *(uintptr_t*)(resume_thread) = (uintptr_t)&Hooks::hk_resume_thread;*/

    //TODO: fix this, this doesn't seem to work for me, i don't know why.
    /* init4.callbackroutine = PointersReassigned;
     PointerReassignmentPluginID = Exported.RegisterFunction(pluginid, ptFunctionPointerchange,
                                                             &init4); //adds a plugin menu item to the memory view
     if (PointerReassignmentPluginID == -1) {
         Exported.ShowMessage((char *) "Failure to register the pointer reassignment plugin");
         return FALSE;
     }*/

    init1.name = (char *) "DMA Methicc APEX CE Plugin";
    init1.callbackroutine = mainmenuplugin;
    ef->RegisterFunction(pluginid, ptMainMenu, &init1);
    printf("Initialized Methicc's CE DMA plugin\n");
    Exported = *ef;
    return TRUE;
}

BOOL __stdcall CEPlugin_DisablePlugin(void) {
    exit(0);
    return TRUE;
}
