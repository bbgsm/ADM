#include "hooks.h"

struct ProcessData {
    DWORD dwSize;
    DWORD th32ParentProcessID;    // this process's parent process
    DWORD th32ProcessID;          // this process
    CHAR szExeFile[MAX_PATH];    // Path
};
std::vector<ProcessData> processDatas;
std::vector<std::string> names = {
        "r5apex.exe",
};
namespace Hooks {
    DWORD current_process = 0;
    PVMMDLL_PROCESS_INFORMATION info = NULL;

    HANDLE hk_create_tool_help_32_snapshot(DWORD dwFlags, DWORD th32ProcessID) {
        return (HANDLE) 0x66;
    }

    BOOL hk_process_32_first(HANDLE hSnapshot, LPPROCESSENTRY32 lppe) {
        info = NULL;
        DWORD count_processes = 0;
        processDatas.clear();
        if (!VMMDLL_ProcessGetInformationAll(memoryTools.vHandle, &info, &count_processes))
            return false;
        for (int i = 0; i < count_processes; ++i) {
            bool next = false;
            for (const auto &name: names) {

                if (strcmp(name.c_str(), info[i].szNameLong) == 0) {
                    next = true;
                    break;
                }
            }
            if (!next) {
                continue;
            }
            if (processDatas.empty()) {
                lppe->dwSize = sizeof(PROCESSENTRY32);
                lppe->th32ParentProcessID = info[i].dwPPID;
                lppe->th32ProcessID = info[i].dwPID;
                strcpy(lppe->szExeFile, info[i].szNameLong);
            }
            ProcessData processData{};
            processData.dwSize = sizeof(PROCESSENTRY32);
            processData.th32ParentProcessID = info[i].dwPPID;
            processData.th32ProcessID = info[i].dwPID;
            strcpy(processData.szExeFile, info[i].szNameLong);
            processDatas.push_back(processData);
        }
        current_process++;
        return true;
    }

    BOOL hk_process_32_next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe) {
        if (current_process >= processDatas.size()) {
            current_process = 0;
            return false;
        }
        lppe->dwSize = sizeof(PROCESSENTRY32);
        lppe->th32ParentProcessID = processDatas[current_process].th32ParentProcessID;
        lppe->th32ProcessID = processDatas[current_process].th32ProcessID;
        strcpy(lppe->szExeFile, processDatas[current_process].szExeFile);
        current_process++;
        return true;
    }
}
