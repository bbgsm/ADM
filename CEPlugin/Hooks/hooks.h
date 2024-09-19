#pragma once
#include "CheatEngine/cepluginsdk.h"
#include "DmaMemoryTools.h"
#include <TlHelp32.h>

inline DmaMemoryTools memoryTools;

namespace Hooks
{
	enum KPROCESSOR_MODE
	{
		KernelMode,
		UserMode,
	};

	//Mem.cpp
	extern SIZE_T hk_virtual_query(HANDLE hProcess, LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
	extern bool hk_write(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);
	extern BOOL hk_read(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);
	extern UINT_PTR __stdcall GetAddressFromPointer(UINT_PTR baseaddress, int offsetcount, int* offsets);
    extern HANDLE hk_open_process(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);

	//Process.cpp
	extern HANDLE hk_create_tool_help_32_snapshot(DWORD dwFlags, DWORD th32ProcessID);
	extern BOOL hk_process_32_first(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
	extern BOOL hk_process_32_next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);

	//Modules.cpp
	extern BOOL hk_module_32_next(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
	extern BOOL hk_module_32_first(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

	//Threads.cpp
	extern BOOL hk_thread_32_next(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
	extern BOOL hk_thread_32_first(HANDLE hSnapshot, LPTHREADENTRY32 lpte);

	//Something still relies on OpenThread making this not working properly... gotto figure out what it is and implement it.
	extern HANDLE hk_open_thread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);
	extern BOOL hk_get_thread_context(HANDLE hThread, PCONTEXT pContext);
	extern DWORD hk_resume_thread(HANDLE hThread);
	extern DWORD hk_suspend_thread(HANDLE hThread);
	extern BOOL hk_set_thread_context(HANDLE hThread, PCONTEXT pContext);
}
