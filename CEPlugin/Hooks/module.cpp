#include "hooks.h"

namespace Hooks {
    PVMMDLL_MAP_MODULE module_info = NULL;
    DWORD current_module = 0;

    BOOL hk_module_32_first(HANDLE hSnapshot, LPMODULEENTRY32 lpme) {
        module_info = NULL;
        current_module = 0;
        if (!VMMDLL_Map_GetModuleU(memoryTools.vHandle, memoryTools.processID, &module_info, VMMDLL_MODULE_FLAG_NORMAL))
            return false;

        lpme->dwSize = sizeof(MODULEENTRY32);
        lpme->th32ProcessID = memoryTools.processID;
        lpme->hModule = (HMODULE) module_info->pMap[current_module].vaBase;
        lpme->modBaseSize = module_info->pMap[current_module].cbImageSize;
        lpme->modBaseAddr = (BYTE *) module_info->pMap[current_module].vaBase;
        strcpy(lpme->szModule, module_info->pMap[current_module].uszText);
        strcpy(lpme->szExePath, module_info->pMap[current_module].uszText);
        return true;
    }

    BOOL hk_module_32_next(HANDLE hSnapshot, LPMODULEENTRY32 lpme) {
        if (current_module >= module_info->cMap) {
            current_module = 0;
            return false;
        }
        lpme->dwSize = sizeof(MODULEENTRY32);
        lpme->th32ProcessID = memoryTools.processID;
        lpme->hModule = (HMODULE) module_info->pMap[current_module].vaBase;
        lpme->modBaseSize = module_info->pMap[current_module].cbImageSize;
        lpme->modBaseAddr = (BYTE *) module_info->pMap[current_module].vaBase;
        strcpy(lpme->szModule, module_info->pMap[current_module].uszText);
        strcpy(lpme->szExePath, module_info->pMap[current_module].uszText);
        current_module++;
        return true;
    }
}
