/*
 cepluginsdk.h
 Updated July 4, 2017

 v5.0.0
*/
#ifndef CEPLUGINSDK_H
#define CEPLUGINSDK_H
#include <winsock2.h>
#include <Windows.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


#define CESDK_VERSION 6

typedef enum {ptAddressList=0, ptMemoryView=1, ptOnDebugEvent=2, ptProcesswatcherEvent=3, ptFunctionPointerchange=4, ptMainMenu=5, ptDisassemblerContext=6, ptDisassemblerRenderLine=7, ptAutoAssembler=8} PluginType;
typedef enum {aaInitialize=0, aaPhase1=1, aaPhase2=2, aaFinalize=3} AutoAssemblerPhase;

typedef struct _PluginVersion
{
    unsigned int version; //write here the minimum version this dll is compatible with (Current supported version: 1 and 2: this SDK only describes 2)
    char *pluginname;  //make this point to a 0-terminated string (allocated memory or static addressin your dll, not stack)
} PluginVersion, *PPluginVersion;

typedef struct _PLUGINTYPE0_RECORD
{
    char *interpretedaddress; //pointer to a 255 bytes long string (0 terminated)
    UINT_PTR address;//this is a read-only representaion of the address. Change interpretedaddress if you want to change this
    BOOL ispointer; //readonly
    int countoffsets; //readonly
    ULONG *offsets; //array of dwords ranging from 0 to countoffsets-1 (readonly)
    char *description; //pointer to a 255 bytes long string
    char valuetype; //0=byte, 1=word, 2=dword, 3=float, 4=double, 5=bit, 6=int64, 7=string
    char size; //stringlength or bitlength;
} PLUGINTYPE0_RECORD, ADDRESSLIST_RECORD, *PPLUGINTYPE0_RECORD, *PADDRESSLIST_RECORD;

//callback routines efinitions for registered plugin functions:
typedef BOOL (__stdcall *CEP_PLUGINTYPE0)(PPLUGINTYPE0_RECORD SelectedRecord);
typedef BOOL (__stdcall *CEP_PLUGINTYPE1)(UINT_PTR *disassembleraddress, UINT_PTR *selected_disassembler_address, UINT_PTR *hexviewaddress);
typedef int (__stdcall *CEP_PLUGINTYPE2)(LPDEBUG_EVENT DebugEvent);
typedef void (__stdcall *CEP_PLUGINTYPE3)(ULONG processid, ULONG peprocess, BOOL Created);
typedef void (__stdcall *CEP_PLUGINTYPE4)(int reserved);
typedef void (__stdcall *CEP_PLUGINTYPE5)(void);
typedef BOOL (__stdcall *CEP_PLUGINTYPE6ONPOPUP)(UINT_PTR selectedAddress, char **addressofname, BOOL *show);
typedef BOOL (__stdcall *CEP_PLUGINTYPE6)(UINT_PTR *selectedAddress);
typedef void (__stdcall *CEP_PLUGINTYPE7)(UINT_PTR address, char **addressStringPointer, char **bytestringpointer, char **opcodestringpointer, char **specialstringpointer, ULONG *textcolor);
typedef void (__stdcall *CEP_PLUGINTYPE8)(char **line, AutoAssemblerPhase phase, int id);



typedef struct _PLUGINTYPE0_INIT
{
    char* name; //0 terminated string describing the name for the user's menu item
    CEP_PLUGINTYPE0 callbackroutine; //pointer to a callback routine of the type 0 plugin
} PLUGINTYPE0_INIT, ADDRESSLISTPLUGIN_INIT, *PPLUGINTYPE0_INIT, *PADDRESSLISTPLUGIN_INIT;

typedef struct _PLUGINTYPE1_INIT
{
    char* name; //0 terminated string describing the name for the user's menu item
    CEP_PLUGINTYPE1 callbackroutine; //pointer to a callback routine of the type 1 plugin
    char* shortcut; //0 terminated string containing the shortcut in textform. CE will try it's best to parse it to a valid shortcut
} PLUGINTYPE1_INIT, MEMORYVIEWPLUGIN_INIT, *PPLUGINTYPE1_INIT, *PMEMORYVIEWPLUGIN_INIT;

typedef struct _PLUGINTYPE2_INIT
{
    CEP_PLUGINTYPE2 callbackroutine; //pointer to a callback routine of the type 2 plugin
} PLUGINTYPE2_INIT, DEBUGEVENTPLUGIN_INIT, *PPLUGINTYPE2_INIT, *PDEBUGEVENTPLUGIN_INIT;

typedef struct _PLUGINTYPE3_INIT
{
    CEP_PLUGINTYPE3 callbackroutine; //pointer to a callback routine of the type 3 plugin
} PLUGINTYPE3_INIT, PROCESSWATCHERPLUGIN_INIT, *PPLUGINTYPE3_INIT, *PPROCESSWATCHERPLUGIN_INIT;

typedef struct _PLUGINTYPE4_INIT
{
    CEP_PLUGINTYPE4 callbackroutine; //pointer to a callback routine of the type 4 plugin
} PLUGINTYPE4_INIT, POINTERREASSIGNMENTPLUGIN_INIT, *PPLUGINTYPE4_INIT, *PPOINTERREASSIGNMENTPLUGIN_INIT;

typedef struct _PLUGINTYPE5_INIT
{
    char* name; //0 terminated string describing the name for the user's menu item
    CEP_PLUGINTYPE5 callbackroutine;
    char* shortcut; //0 terminated string containing the shortcut in textform. CE will try it's best to parse it to a valid shortcut
} PLUGINTYPE5_INIT, MAINMENUPLUGIN_INIT, *PPLUGINTYPE5_INIT, *PMAINMENUPLUGIN_INIT;

typedef struct _PLUGINTYPE6_INIT
{
    char* name; //0 terminated string describing the name for the user's menu item
    CEP_PLUGINTYPE6 callbackroutine;
    CEP_PLUGINTYPE6ONPOPUP callbackroutineOnPopup;
    char* shortcut; //0 terminated string containing the shortcut in textform. CE will try it's best to parse it to a valid shortcut
} PLUGINTYPE6_INIT, DISASSEMBLERCONTEXT_INIT, *PPLUGINTYPE6_INIT, *PDISASSEMBLERCONTEXT_INIT;

typedef struct _PLUGINTYPE7_INIT
{
    CEP_PLUGINTYPE7 callbackroutine; //pointer to a callback routine of the type 7 plugin
} PLUGINTYPE7_INIT, DISASSEMBLERLINEPLUGIN_INIT, *PPLUGINTYPE7_INIT, *PDISASSEMBLERLINEPLUGIN_INIT;

typedef struct _PLUGINTYPE8_INIT
{
    CEP_PLUGINTYPE8 callbackroutine; //pointer to a callback routine of the type 8 plugin
} PLUGINTYPE8_INIT, AUTOASSEMBLERPLUGIN_INIT, *PPLUGINTYPE8_INIT, *PAUTOASSEMBLERPLUGIN_INIT;

typedef struct _REGISTERMODIFICATIONINFO
{
    UINT_PTR address; //addres to break on
    BOOL change_eax;
    BOOL change_ebx;
    BOOL change_ecx;
    BOOL change_edx;
    BOOL change_esi;
    BOOL change_edi;
    BOOL change_ebp;
    BOOL change_esp;
    BOOL change_eip;
#ifdef _AMD64_
    BOOL change_r8;
    BOOL change_r9;
    BOOL change_r10;
    BOOL change_r11;
    BOOL change_r12;
    BOOL change_r13;
    BOOL change_r14;
    BOOL change_r15;
#endif
    BOOL change_cf;
    BOOL change_pf;
    BOOL change_af;
    BOOL change_zf;
    BOOL change_sf;
    BOOL change_of;
    UINT_PTR new_eax;
    UINT_PTR new_ebx;
    UINT_PTR new_ecx;
    UINT_PTR new_edx;
    UINT_PTR new_esi;
    UINT_PTR new_edi;
    UINT_PTR new_ebp;
    UINT_PTR new_esp;
    UINT_PTR new_eip;
#ifdef _AMD64_
    UINT_PTR new_r8;
    UINT_PTR new_r9;
    UINT_PTR new_r10;
    UINT_PTR new_r11;
    UINT_PTR new_r12;
    UINT_PTR new_r13;
    UINT_PTR new_r14;
    UINT_PTR new_r15;
#endif


    BOOL new_cf;
    BOOL new_pf;
    BOOL new_af;
    BOOL new_zf;
    BOOL new_sf;
    BOOL new_of;
} REGISTERMODIFICATIONINFO, *PREGISTERMODIFICATIONINFO;

//the __stdcall stuff isn't really needed since I've set compiler options to force stdcall, but this makes it clear that stdcall is used to the reader
typedef void (__stdcall *CEP_SHOWMESSAGE)(char* message);
typedef int (__stdcall *CEP_REGISTERFUNCTION) (int pluginid, PluginType functiontype, PVOID init);
typedef BOOL (__stdcall *CEP_UNREGISTERFUNCTION) (int pluginid, int functionid);
typedef HANDLE (__stdcall *CEP_GETMAINWINDOWHANDLE) (void);
typedef BOOL (__stdcall *CEP_AUTOASSEMBLE) (char *script);
typedef BOOL (__stdcall *CEP_ASSEMBLER) (UINT_PTR address, char* instruction, BYTE *output, int maxlength, int *returnedsize);
typedef BOOL (__stdcall *CEP_DISASSEMBLER) (UINT_PTR address, char* output, int maxsize);
typedef BOOL (__stdcall *CEP_CHANGEREGATADDRESS) (UINT_PTR address,PREGISTERMODIFICATIONINFO changereg);
typedef BOOL (__stdcall *CEP_INJECTDLL) (char *dllname, char *functiontocall);
typedef int (__stdcall *CEP_FREEZEMEM) (UINT_PTR address, int size);
typedef BOOL (__stdcall *CEP_UNFREEZEMEM) (int freezeID);
typedef BOOL (__stdcall *CEP_FIXMEM) (void);
typedef BOOL (__stdcall *CEP_PROCESSLIST) (char *listbuffer, int listsize);
typedef BOOL (__stdcall *CEP_RELOADSETTINGS) (void);
typedef UINT_PTR (__stdcall *CEP_GETADDRESSFROMPOINTER) (UINT_PTR baseaddress, int offsetcount, int* offsets);
typedef BOOL (__stdcall *CEP_GENERATEAPIHOOKSCRIPT) (char *address, char *addresstojumpto, char *addresstogetnewcalladdress, char *script, int maxscriptsize);
typedef BOOL (__stdcall *CEP_ADDRESSTONAME) (UINT_PTR address, char *name, int maxnamesize);
typedef BOOL (__stdcall *CEP_NAMETOADDRESS) (char *name, UINT_PTR *address);

typedef VOID (__stdcall *CEP_LOADDBK32)(void);
typedef BOOL (__stdcall *CEP_LOADDBVMIFNEEDED)(void);
typedef DWORD (__stdcall *CEP_PREVIOUSOPCODE)(UINT_PTR address);
typedef DWORD (__stdcall *CEP_NEXTOPCODE)(UINT_PTR address);
typedef BOOL (__stdcall *CEP_LOADMODULE)(char *modulepath, char *exportlist, int *maxsize);
typedef BOOL (__stdcall *CEP_DISASSEMBLEEX)(UINT_PTR address, char *output, int maxsize);
typedef VOID (__stdcall *CEP_AA_ADDCOMMAND)(char *command);
typedef VOID (__stdcall *CEP_AA_DELCOMMAND)(char *command);

typedef PVOID (__stdcall *CEP_CREATETABLEENTRY)(void);
typedef PVOID (__stdcall *CEP_GETTABLEENTRY)(char *description);
typedef BOOL (__stdcall *CEP_MEMREC_SETDESCRIPTION)(PVOID memrec, char *description);
typedef PCHAR (__stdcall *CEP_MEMREC_GETDESCRIPTION)(PVOID memrec);
typedef BOOL (__stdcall *CEP_MEMREC_GETADDRESS)(PVOID memrec, UINT_PTR *address, DWORD *offsets, int maxoffsets, int *neededOffsets);
typedef BOOL (__stdcall *CEP_MEMREC_SETADDRESS)(PVOID memrec, char *address, DWORD *offsets, int offsetcount);
typedef int (__stdcall *CEP_MEMREC_GETTYPE)(PVOID memrec);
typedef BOOL (__stdcall *CEP_MEMREC_SETTYPE)(PVOID memrec, int vtype);
typedef BOOL (__stdcall *CEP_MEMREC_GETVALUETYPE)(PVOID memrec, char *value, int maxsize);
typedef BOOL (__stdcall *CEP_MEMREC_SETVALUETYPE)(PVOID memrec, char *value);
typedef char* (__stdcall *CEP_MEMREC_GETSCRIPT)(PVOID memrec);
typedef BOOL (__stdcall *CEP_MEMREC_SETSCRIPT)(PVOID memrec, char *script);
typedef BOOL (__stdcall *CEP_MEMREC_ISFROZEN)(PVOID memrec);
typedef BOOL (__stdcall *CEP_MEMREC_FREEZE)(PVOID memrec, int direction);
typedef BOOL (__stdcall *CEP_MEMREC_UNFREEZE)(PVOID memrec);
typedef BOOL (__stdcall *CEP_MEMREC_SETCOLOR)(PVOID memrec, DWORD color);
typedef BOOL (__stdcall *CEP_MEMREC_APPENDTOENTRY)(PVOID memrec1, PVOID memrec2);
typedef BOOL (__stdcall *CEP_MEMREC_DELETE)(PVOID memrec);

typedef DWORD (__stdcall *CEP_GETPROCESSIDFROMPROCESSNAME)(char *name);
typedef DWORD (__stdcall *CEP_OPENPROCESS)(DWORD pid);
typedef DWORD (__stdcall *CEP_DEBUGPROCESS)(int debuggerinterface);
typedef VOID (__stdcall *CEP_PAUSE)(void);
typedef VOID (__stdcall *CEP_UNPAUSE)(void);
typedef BOOL (__stdcall *CEP_DEBUG_SETBREAKPOINT)(UINT_PTR address, int size, int trigger);
typedef BOOL (__stdcall *CEP_DEBUG_REMOVEBREAKPOINT)(UINT_PTR address);
typedef BOOL (__stdcall *CEP_DEBUG_CONTINUEFROMBREAKPOINT)(int continueoption);

typedef VOID (__stdcall *CEP_CLOSECE)(void);
typedef VOID (__stdcall *CEP_HIDEALLCEWINDOWS)(void);
typedef VOID (__stdcall *CEP_UNHIDEMAINCEWINDOW)(void);

typedef PVOID (__stdcall *CEP_CREATEFORM)(void);
typedef void (__stdcall *CEP_FORM_CENTERSCREEN)(PVOID form);
typedef void (__stdcall *CEP_FORM_HIDE)(PVOID form);
typedef void (__stdcall *CEP_FORM_SHOW)(PVOID form);
typedef void (__stdcall *CEP_FORM_ONCLOSE)(PVOID form, PVOID function);

typedef PVOID (__stdcall *CEP_CREATEPANEL)(PVOID owner);
typedef PVOID (__stdcall *CEP_CREATEGROUPBOX)(PVOID owner);
typedef PVOID (__stdcall *CEP_CREATEBUTTON)(PVOID owner);
typedef PVOID (__stdcall *CEP_CREATEIMAGE)(PVOID owner);

typedef BOOL (__stdcall *CEP_IMAGE_LOADIMAGEFROMFILE)(PVOID image, char *filename);
typedef VOID (__stdcall *CEP_IMAGE_TRANSPARENT)(PVOID image, BOOL transparent);
typedef VOID (__stdcall *CEP_IMAGE_STRETCH)(PVOID image, BOOL stretch);

typedef PVOID (__stdcall *CEP_CREATELABEL)(PVOID owner);
typedef PVOID (__stdcall *CEP_CREATEEDIT)(PVOID owner);
typedef PVOID (__stdcall *CEP_CREATEMEMO)(PVOID owner);
typedef PVOID (__stdcall *CEP_CREATETIMER)(PVOID owner);

typedef VOID (__stdcall *CEP_TIMER_SETINTERVAL)(PVOID timer, int interval);
typedef VOID (__stdcall *CEP_TIMER_ONTIMER)(PVOID timer, PVOID function);

typedef VOID (__stdcall *CEP_CONTROL_SETCAPTION)(PVOID control, char *caption);
typedef BOOL (__stdcall *CEP_CONTROL_GETCAPTION)(PVOID control, char *caption, int maxsize);

typedef VOID (__stdcall *CEP_CONTROL_SETPOSITION)(PVOID control, int x, int y);
typedef int (__stdcall *CEP_CONTROL_GETX)(PVOID control);
typedef int (__stdcall *CEP_CONTROL_GETY)(PVOID control);

typedef VOID (__stdcall *CEP_CONTROL_SETSIZE)(PVOID control, int width, int height);
typedef int (__stdcall *CEP_CONTROL_GETWIDTH)(PVOID control);
typedef int (__stdcall *CEP_CONTROL_GETHEIGHT)(PVOID control);

typedef VOID (__stdcall *CEP_CONTROL_SETALIGN)(PVOID control, int align);
typedef VOID (__stdcall *CEP_CONTROL_ONCLICK)(PVOID control, PVOID function);

typedef VOID (__stdcall *CEP_OBJECT_DESTROY)(PVOID object);

typedef int (__stdcall *CEP_MESSAGEDIALOG)(char *massage, int messagetype, int buttoncombination);
typedef BOOL (__stdcall *CEP_SPEEDHACK_SETSPEED)(float speed);
typedef lua_State *(__fastcall *CEP_GETLUASTATE)();


typedef BOOL(__stdcall **CEP_READPROCESSMEMORY)(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T * lpNumberOfBytesRead);

/*
function ce_messageDialog(message: pchar; messagetype: integer; buttoncombination: integer): integer; stdcall;
function ce_speedhack_setSpeed(speed: single): BOOL; stdcall;
*/

typedef struct _ExportedFunctions
{
    int sizeofExportedFunctions;              // ExportedFunctions 结构体的大小，确保插件与 Cheat Engine 使用相同的结构体版本
    CEP_SHOWMESSAGE ShowMessage;              // 指向 Cheat Engine 的 ShowMessage 函数，用于显示消息框
    CEP_REGISTERFUNCTION RegisterFunction;    // 用于注册特定类型的插件功能
    CEP_UNREGISTERFUNCTION UnregisterFunction;// 取消注册通过 RegisterFunction 注册的功能
    PULONG OpenedProcessID;                   // 指向当前选择的进程ID的指针
    PHANDLE OpenedProcessHandle;              // 指向当前选择的进程句柄的指针

    CEP_GETMAINWINDOWHANDLE GetMainWindowHandle; // 返回 Cheat Engine 主窗口句柄
    CEP_AUTOASSEMBLE AutoAssemble;               // 指向 AutoAssemble 函数的指针，用于执行自动汇编脚本
    CEP_ASSEMBLER Assembler;                     // 指向汇编器函数的指针，用于将汇编代码编译为机器代码
    CEP_DISASSEMBLER Disassembler;               // 指向反汇编器函数的指针，用于将机器代码反汇编为汇编代码
    CEP_CHANGEREGATADDRESS ChangeRegistersAtAddress; // 指向 ChangeRegAtBP 函数的指针，用于在指定地址更改寄存器值
    CEP_INJECTDLL InjectDLL;                    // 指向注入 DLL 函数的指针
    CEP_FREEZEMEM FreezeMem;                    // 指向 FreezeMem 函数的指针，用于冻结内存
    CEP_UNFREEZEMEM UnfreezeMem;                // 指向 UnfreezeMem 函数的指针，用于解冻内存
    CEP_FIXMEM FixMem;                          // 指向修复内存功能的指针
    CEP_PROCESSLIST ProcessList;                // 指向进程列表函数的指针，用于获取系统中所有进程的列表
    CEP_RELOADSETTINGS ReloadSettings;          // 指向重新加载设置函数的指针，用于重新加载 Cheat Engine 的配置
    CEP_GETADDRESSFROMPOINTER GetAddressFromPointer; // 指向从指针获取地址的函数，用于多级指针解析

    // 指向包含函数指针的地址的指针
    // 可通过更改这些指针来挂钩API，或者直接使用这些API
    CEP_READPROCESSMEMORY ReadProcessMemory;    // 指向 ReadProcessMemory 函数指针的指针
    PVOID WriteProcessMemory;                   // 指向 WriteProcessMemory 函数指针的指针
    PVOID GetThreadContext;                     // 指向 GetThreadContext 函数指针的指针
    PVOID SetThreadContext;                     // 指向 SetThreadContext 函数指针的指针
    PVOID SuspendThread;                        // 指向 SuspendThread 函数指针的指针
    PVOID ResumeThread;                         // 指向 ResumeThread 函数指针的指针
    PVOID OpenProcess;                          // 指向 OpenProcess 函数指针的指针
    PVOID WaitForDebugEvent;                    // 指向 WaitForDebugEvent 函数指针的指针
    PVOID ContinueDebugEvent;                   // 指向 ContinueDebugEvent 函数指针的指针
    PVOID DebugActiveProcess;                   // 指向 DebugActiveProcess 函数指针的指针
    PVOID StopDebugging;                        // 指向 StopDebugging 函数指针的指针
    PVOID StopRegisterChange;                   // 指向 StopRegisterChange 函数指针的指针
    PVOID VirtualProtect;                       // 指向 VirtualProtect 函数指针的指针
    PVOID VirtualProtectEx;                     // 指向 VirtualProtectEx 函数指针的指针
    PVOID VirtualQueryEx;                       // 指向 VirtualQueryEx 函数指针的指针
    PVOID VirtualAllocEx;                       // 指向 VirtualAllocEx 函数指针的指针
    PVOID CreateRemoteThread;                   // 指向 CreateRemoteThread 函数指针的指针
    PVOID OpenThread;                           // 指向 OpenThread 函数指针的指针
    PVOID GetPEProcess;                         // 指向获取PE进程结构的函数指针
    PVOID GetPEThread;                          // 指向获取PE线程结构的函数指针
    PVOID GetThreadsProcessOffset;              // 指向获取线程进程偏移的函数指针
    PVOID GetThreadListEntryOffset;             // 指向获取线程列表条目偏移的函数指针
    PVOID GetProcessnameOffset;                 // 指向获取进程名称偏移的函数指针
    PVOID GetDebugportOffset;                   // 指向获取调试端口偏移的函数指针
    PVOID GetPhysicalAddress;                   // 指向获取物理地址的函数指针
    PVOID ProtectMe;                            // 指向保护进程自身不被调试的函数指针
    PVOID GetCR4;                               // 指向获取CR4寄存器的函数指针
    PVOID GetCR3;                               // 指向获取CR3寄存器的函数指针
    PVOID SetCR3;                               // 指向设置CR3寄存器的函数指针
    PVOID GetSDT;                               // 指向获取系统描述符表的函数指针
    PVOID GetSDTShadow;                         // 指向获取影子系统描述符表的函数指针
    PVOID setAlternateDebugMethod;              // 指向设置备用调试方法的函数指针
    PVOID getAlternateDebugMethod;              // 指向获取备用调试方法的函数指针
    PVOID DebugProcess;                         // 指向调试进程的函数指针
    PVOID ChangeRegOnBP;                        // 指向在断点时更改寄存器值的函数指针
    PVOID RetrieveDebugData;                    // 指向检索调试数据的函数指针
    PVOID StartProcessWatch;                    // 指向启动进程监视功能的函数指针
    PVOID WaitForProcessListData;               // 指向等待进程列表数据更新的函数指针
    PVOID GetProcessNameFromID;                 // 指向通过进程ID获取进程名称的函数指针
    PVOID GetProcessNameFromPEProcess;          // 指向通过PE进程结构获取进程名称的函数指针
    PVOID KernelOpenProcess;                    // 指向内核模式下打开进程的函数指针
    PVOID KernelReadProcessMemory;              // 指向内核模式下读取进程内存的函数指针
    PVOID KernelWriteProcessMemory;             // 指向内核模式下写入进程内存的函数指针
    PVOID KernelVirtualAllocEx;                 // 指向内核模式下分配虚拟内存的函数指针
    PVOID IsValidHandle;                        // 指向检查句柄有效性的函数指针
    PVOID GetIDTCurrentThread;                  // 指向获取当前线程IDT的函数指针
    PVOID GetIDTs;                              // 指向获取所有IDT的函数指针
    PVOID MakeWritable;                         // 指向将内存页设置为可写的函数指针
    PVOID GetLoadedState;                       // 指向获取加载状态的函数指针
    PVOID DBKSuspendThread;                     // 指向DBK模式下挂起线程的函数指针
    PVOID DBKResumeThread;                      // 指向DBK模式下恢复线程的函数指针
    PVOID DBKSuspendProcess;                    // 指向DBK模式下挂起进程的函数指针
    PVOID DBKResumeProcess;                     // 指向DBK模式下恢复进程的函数指针
    PVOID KernelAlloc;                          // 指向内核模式下分配内存的函数指针
    PVOID GetKProcAddress;                      // 指向获取内核函数地址的函数指针
    PVOID CreateToolhelp32Snapshot;             // 指向创建系统快照的函数指针（类似于 Windows 的 CreateToolhelp32Snapshot）
    PVOID Process32First;                       // 指向获取系统中第一个进程信息的函数指针
    PVOID Process32Next;                        // 指向获取系统中下一个进程信息的函数指针
    PVOID Thread32First;                        // 指向获取系统中第一个线程信息的函数指针
    PVOID Thread32Next;                         // 指向获取系统中下一个线程信息的函数指针
    PVOID Module32First;                        // 指向获取系统中第一个模块信息的函数指针
    PVOID Module32Next;                         // 指向获取系统中下一个模块信息的函数指针
    PVOID Heap32ListFirst;                      // 指向获取系统中第一个堆列表信息的函数指针
    PVOID Heap32ListNext;                       // 指向获取系统中下一个堆列表信息的函数指针

    // 高级功能，仅限 Delphi 7 企业版 DLL 编程人员使用
    PVOID mainform;                             // 指向 TMainForm 对象的指针（主窗口）
    PVOID memorybrowser;                        // 指向 TMemoryBrowser 对象的指针（内存查看窗口）

    // 插件版本 2+ 的扩展功能
    CEP_NAMETOADDRESS sym_nameToAddress;        // 指向符号名称到地址的转换函数的指针
    CEP_ADDRESSTONAME sym_addressToName;        // 指向地址到符号名称的转换函数的指针
    CEP_GENERATEAPIHOOKSCRIPT sym_generateAPIHookScript; // 指向生成 API Hook 脚本的函数指针

    // 插件版本 3+ 的扩展功能
    CEP_LOADDBK32 loadDBK32;                    // 指向加载 DBK32 驱动的函数指针
    CEP_LOADDBVMIFNEEDED loaddbvmifneeded;      // 指向必要时加载 DBVM 的函数指针
    CEP_PREVIOUSOPCODE previousOpcode;          // 指向获取前一个操作码的函数指针
    CEP_NEXTOPCODE nextOpcode;                  // 指向获取下一个操作码的函数指针
    CEP_DISASSEMBLEEX disassembleEx;            // 指向扩展反汇编函数的指针
    CEP_LOADMODULE loadModule;                  // 指向加载模块的函数指针
    CEP_AA_ADDCOMMAND aa_AddExtraCommand;       // 指向添加额外汇编命令的函数指针
    CEP_AA_DELCOMMAND aa_RemoveExtraCommand;    // 指向删除额外汇编命令的函数指针

    // 插件版本 4 的扩展功能
    CEP_CREATETABLEENTRY createTableEntry;      // 指向创建表项的函数指针
    CEP_GETTABLEENTRY getTableEntry;            // 指向获取表项的函数指针
    CEP_MEMREC_SETDESCRIPTION memrec_setDescription; // 指向设置内存记录描述的函数指针
    CEP_MEMREC_GETDESCRIPTION memrec_getDescription; // 指向获取内存记录描述的函数指针
    CEP_MEMREC_GETADDRESS memrec_getAddress;    // 指向获取内存记录地址的函数指针
    CEP_MEMREC_SETADDRESS memrec_setAddress;    // 指向设置内存记录地址的函数指针
    CEP_MEMREC_GETTYPE memrec_getType;          // 指向获取内存记录类型的函数指针
    CEP_MEMREC_SETTYPE memrec_setType;          // 指向设置内存记录类型的函数指针
    CEP_MEMREC_GETVALUETYPE memrec_getValue;    // 指向获取内存记录值的函数指针
    CEP_MEMREC_SETVALUETYPE memrec_setValue;    // 指向设置内存记录值的函数指针
    CEP_MEMREC_GETSCRIPT memrec_getScript;      // 指向获取内存记录脚本的函数指针
    CEP_MEMREC_SETSCRIPT memrec_setScript;      // 指向设置内存记录脚本的函数指针
    CEP_MEMREC_ISFROZEN memrec_isfrozen;        // 指向检查内存记录是否被冻结的函数指针
    CEP_MEMREC_FREEZE memrec_freeze;            // 指向冻结内存记录的函数指针
    CEP_MEMREC_UNFREEZE memrec_unfreeze;        // 指向解冻内存记录的函数指针
    CEP_MEMREC_SETCOLOR memrec_setColor;        // 指向设置内存记录颜色的函数指针
    CEP_MEMREC_APPENDTOENTRY memrec_appendtoentry; // 指向将内存记录附加到现有表项的函数指针
    CEP_MEMREC_DELETE memrec_delete;            // 指向删除内存记录的函数指针

    CEP_GETPROCESSIDFROMPROCESSNAME getProcessIDFromProcessName; // 指向通过进程名称获取进程ID的函数指针
    CEP_OPENPROCESS openProcessEx;              // 指向打开进程的扩展函数指针
    CEP_DEBUGPROCESS debugProcessEx;            // 指向调试进程的扩展函数指针
    CEP_PAUSE pause;                            // 指向暂停进程的函数指针
    CEP_UNPAUSE unpause;                        // 指向继续进程的函数指针

    CEP_DEBUG_SETBREAKPOINT debug_setBreakpoint; // 指向设置断点的函数指针
    CEP_DEBUG_REMOVEBREAKPOINT debug_removeBreakpoint; // 指向移除断点的函数指针
    CEP_DEBUG_CONTINUEFROMBREAKPOINT debug_continueFromBreakpoint; // 指向从断点继续执行的函数指针

    CEP_CLOSECE closeCE;                        // 指向关闭 Cheat Engine 的函数指针
    CEP_HIDEALLCEWINDOWS hideAllCEWindows;      // 指向隐藏所有 Cheat Engine 窗口的函数指针
    CEP_UNHIDEMAINCEWINDOW unhideMainCEwindow;  // 指向取消隐藏主 Cheat Engine 窗口的函数指针
    CEP_CREATEFORM createForm;                  // 指向创建表单的函数指针
    CEP_FORM_CENTERSCREEN form_centerScreen;    // 指向将表单居中的函数指针
    CEP_FORM_HIDE form_hide;                    // 指向隐藏表单的函数指针
    CEP_FORM_SHOW form_show;                    // 指向显示表单的函数指针
    CEP_FORM_ONCLOSE form_onClose;              // 指向表单关闭事件的处理函数指针

    CEP_CREATEPANEL createPanel;                // 指向创建面板的函数指针
    CEP_CREATEGROUPBOX createGroupBox;          // 指向创建分组框的函数指针
    CEP_CREATEBUTTON createButton;              // 指向创建按钮的函数指针
    CEP_CREATEIMAGE createImage;                // 指向创建图像的函数指针
    CEP_IMAGE_LOADIMAGEFROMFILE image_loadImageFromFile; // 指向从文件加载图像的函数指针
    CEP_IMAGE_TRANSPARENT image_transparent;    // 指向设置图像透明度的函数指针
    CEP_IMAGE_STRETCH image_stretch;            // 指向设置图像拉伸属性的函数指针

    CEP_CREATELABEL createLabel;                // 指向创建标签的函数指针
    CEP_CREATEEDIT createEdit;                  // 指向创建编辑框的函数指针
    CEP_CREATEMEMO createMemo;                  // 指向创建多行文本框的函数指针
    CEP_CREATETIMER createTimer;                // 指向创建定时器的函数指针
    CEP_TIMER_SETINTERVAL timer_setInterval;    // 指向设置定时器间隔的函数指针
    CEP_TIMER_ONTIMER timer_onTimer;            // 指向定时器触发事件的处理函数指针
    CEP_CONTROL_SETCAPTION control_setCaption;  // 指向设置控件标题的函数指针
    CEP_CONTROL_GETCAPTION control_getCaption;  // 指向获取控件标题的函数指针
    CEP_CONTROL_SETPOSITION control_setPosition; // 指向设置控件位置的函数指针
    CEP_CONTROL_GETX control_getX;              // 指向获取控件X坐标的函数指针
    CEP_CONTROL_GETY control_getY;              // 指向获取控件Y坐标的函数指针
    CEP_CONTROL_SETSIZE control_setSize;        // 指向设置控件大小的函数指针
    CEP_CONTROL_GETWIDTH control_getWidth;      // 指向获取控件宽度的函数指针
    CEP_CONTROL_GETHEIGHT control_getHeight;    // 指向获取控件高度的函数指针
    CEP_CONTROL_SETALIGN control_setAlign;      // 指向设置控件对齐方式的函数指针
    CEP_CONTROL_ONCLICK control_onClick;        // 指向控件点击事件的处理函数指针

    CEP_OBJECT_DESTROY object_destroy;          // 指向销毁对象的函数指针
    CEP_MESSAGEDIALOG messageDialog;            // 指向消息对话框函数的指针
    CEP_SPEEDHACK_SETSPEED speedhack_setSpeed;  // 指向设置速度修改器速度的函数指针

    // 插件版本 5 的扩展功能（待实现）
    VOID *ExecuteKernelCode;                    // 指向执行内核代码的函数指针
    VOID *UserdefinedInterruptHook;             // 指向用户定义的中断挂钩的函数指针
    CEP_GETLUASTATE GetLuaState;                // 指向获取 Lua 状态的函数指针
    VOID *MainThreadCall;                       // 指向主线程调用函数的指针

} ExportedFunctions, *PExportedFunctions;



BOOL __stdcall CEPlugin_GetVersion(PPluginVersion pv , int sizeofpluginversion);
BOOL __stdcall CEPlugin_InitializePlugin(PExportedFunctions ef , int pluginid);
BOOL __stdcall CEPlugin_DisablePlugin(void);
//old versions without CEPlugin_ in front also work but are not recommended due to bugbrained compilers...


#endif //CEPLUGINSDK_H