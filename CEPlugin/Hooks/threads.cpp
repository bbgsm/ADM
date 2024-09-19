#include "hooks.h"

namespace Hooks {


    HANDLE hk_open_thread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId) {

        return nullptr;
    }

    BOOL hk_get_thread_context(HANDLE hThread, PCONTEXT pContext) {
        return false;
    }

    BOOL hk_set_thread_context(HANDLE hThread, PCONTEXT pContext) {

        return false;
    }

    DWORD hk_suspend_thread(HANDLE hThread) {

        return -1;
    }

    DWORD hk_resume_thread(HANDLE hThread) {


        return -1;
    }

    BOOL hk_thread_32_first(HANDLE hSnapshot, LPTHREADENTRY32 lpte) {

        return true;
    }

    BOOL hk_thread_32_next(HANDLE hSnapshot, LPTHREADENTRY32 lpte) {

        return true;
    }
}
