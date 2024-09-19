#include <list>
#include <map>

#include "DmaMemoryTools.h"
#include "hooks.h"

#include <thread>
#include <vector>
#include "Memory/memmy.h"
#include "Memory/vad.h"
#include "vmmdll.h"

extern void plugin();


namespace Hooks {
    HANDLE hk_open_process(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId) {
        if (memoryTools.init("r5apex.exe")) {
            std::thread t1(plugin);
            t1.detach();
            return (HANDLE)0x69;
        }
        //        if (mem.Init(dwProcessId))
        std::cout << "Failed to initialize DMA" << std::endl;
        return 0;
    }

    UINT_PTR __stdcall GetAddressFromPointer(UINT_PTR baseaddress, int offsetcount, int *offsets) {
        printf("GetAddressFromPointer\n");
        return 0;
    }

    BOOL hk_read(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesRead) {
        *lpNumberOfBytesRead = memoryTools.readV(lpBuffer, (Addr)lpBaseAddress, nSize);
        return *lpNumberOfBytesRead == nSize;
    }

    bool hk_write(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesRead) {
        *lpNumberOfBytesRead = memoryTools.writeV(lpBuffer, (Addr)lpBaseAddress, nSize);
        return *lpNumberOfBytesRead == nSize;
    }

    std::list<c_memory_region<vad_info>> get_memory_region() {
        std::list<c_memory_region<vad_info>> result = {};
        PVMMDLL_MAP_VAD vads = nullptr;

        if (!VMMDLL_Map_GetVadW(memoryTools.vHandle, memoryTools.processID, true, &vads))
            return {};

        std::vector<vad_info> vad_infos;
        for (size_t i = 0; i < vads->cMap; i++) {
            auto vad = vads->pMap[i];
            vad_infos.push_back(vad_info(vad.wszText, vad.vaStart, vad.vaEnd, vad));
        }

        for (size_t i = 0; i < vad_infos.size(); i++) {
            size_t regionSize = vad_infos[i].get_end() - vad_infos[i].get_start() + 1;
            result.push_back(c_memory_region<vad_info>(vad_infos[i], vad_infos[i].get_start(), regionSize));
        }
        return result;
    }

    std::map<int, std::pair<uint64_t, std::list<c_memory_region<vad_info>>>> region_cache;

    bool VirtualQueryImpl_(uintptr_t lpAddress, c_memory_region<vad_info> *ret) {
        if (region_cache.find(memoryTools.processID) != region_cache.end()) {
            auto &&[time, region] = region_cache[memoryTools.processID];
            if (GetTickCount() - time > 1000) {
                auto &&new_region = get_memory_region();
                region_cache.erase(memoryTools.processID);
                region_cache.insert({memoryTools.processID, {GetTickCount(), new_region}});
            }
        }
        else {
            auto &&new_region = get_memory_region();
            region_cache.insert({memoryTools.processID, std::pair(GetTickCount(), new_region)});
        }
        auto regions = region_cache[memoryTools.processID].second;

        auto it = std::lower_bound(
            regions.begin(), regions.end(), lpAddress,
            [](const c_memory_region<vad_info> &region, uintptr_t addr) { return region.get_region_start() <= addr; });
        if (it == regions.end())
            return false;
        *ret = *it;
        return true;
    }

    // Memory in VirtualQuery Is always rounded down, getMemoryRegionContaining will find the nearest (rounded down)
    // region that contains the address. or if it's equal return the exact region.
    PVMMDLL_MAP_PTE pMemMapEntries = NULL;
    PVMMDLL_MAP_MODULE pModuleEntries = NULL;

    SIZE_T hk_virtual_query(HANDLE hProcess, LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength) {
        //        printf("hk_virtual_query: %llX\n",lpAddress);

        MEMORY_BASIC_INFORMATION info{};
        bool valid = false;

        if (lpAddress > (void *)0x7ff000000000) {
            if (!pMemMapEntries) {
                bool result = VMMDLL_Map_GetPte(memoryTools.vHandle, memoryTools.processID, TRUE, &pMemMapEntries);
                if (!result) {
                    printf("Failed to get PTE\n");
                    return 0;
                }
            }
            if (!pModuleEntries) {
                bool result = VMMDLL_Map_GetModule(memoryTools.vHandle, memoryTools.processID, &pModuleEntries, NULL);
                if (!result) {
                    printf("Failed to get Modules\n");
                    return 0;
                }
            }

            for (int i = 0; i < pMemMapEntries->cMap; i++) {
                PVMMDLL_MAP_PTEENTRY memMapEntry = &pMemMapEntries->pMap[i];
                if ((uintptr_t)lpAddress > memMapEntry->vaBase)
                    continue;

                if ((uintptr_t)lpAddress < memMapEntry->vaBase) {
                    // When it's smaller than the base address, it's not in the region we have ot then do regionsize
                    // till the next region printf("Found Region not within page: 0x%llx - 0x%llx - %d\n",
                    // memMapEntry->vaBase, memMapEntry->vaBase + (memMapEntry->cPages << 12), memMapEntry->cPages <<
                    // 12);
                    info.AllocationBase = (PVOID)lpAddress;
                    info.RegionSize = memMapEntry->vaBase - (uintptr_t)lpAddress;
                    info.BaseAddress = (void *)((uintptr_t)lpAddress & ~(0xFF));
                    info.Protect = 1;
                    info.State = MEM_FREE;
                    info.Type = 0;
                    valid = true;
                    break;
                }
                else if ((uintptr_t)lpAddress == memMapEntry->vaBase) {
                    if (info.BaseAddress == 0)
                        info.BaseAddress = (void *)((uintptr_t)memMapEntry->vaBase & ~(0xFF));
                    size_t size = memMapEntry->cPages << 12;
                    if (i + 1 <= pMemMapEntries->cMap)
                        size = pMemMapEntries->pMap[i + 1].vaBase - memMapEntry->vaBase;
                    info.RegionSize = size;
                    info.Protect = 0;
                    info.State = MEM_COMMIT;

                    // This part taken from
                    // https://github.com/imerzan/ReClass-DMA/blob/master/PciLeechPlugin/dllmain.cpp#L112
                    if (memMapEntry->fPage & VMMDLL_MEMMAP_FLAG_PAGE_NS)
                        info.Protect |= PAGE_READONLY;
                    if (memMapEntry->fPage & VMMDLL_MEMMAP_FLAG_PAGE_W)
                        info.Protect |= PAGE_READWRITE;
                    if (!(memMapEntry->fPage & VMMDLL_MEMMAP_FLAG_PAGE_NX))
                        info.Protect |= PAGE_EXECUTE;

                    if (memMapEntry->wszText[0]) {
                        if ((memMapEntry->wszText[0] == 'H' && memMapEntry->wszText[1] == 'E' &&
                             memMapEntry->wszText[2] == 'A' && memMapEntry->wszText[3] == 'P') ||
                            (memMapEntry->wszText[0] == '[' && memMapEntry->wszText[1] == 'H' &&
                             memMapEntry->wszText[2] == 'E' && memMapEntry->wszText[3] == 'A' &&
                             memMapEntry->wszText[4] == 'P')) {
                            info.Type = MEM_PRIVATE;
                        }
                        else {
                            info.Type = MEM_IMAGE;
                        }
                    }
                    else {
                        info.Type = MEM_MAPPED;
                    }
                    info.Protect = PAGE_EXECUTE_READWRITE;
                    info.AllocationBase = (PVOID)lpAddress;
                    info.AllocationProtect = info.Protect;
                    info.PartitionId = 0;
                    valid = true;
                    break;
                }
            }

            if (!valid) {
                pModuleEntries = NULL;
                pMemMapEntries = NULL;
                return false;
            }
            memcpy(lpBuffer, &info, sizeof(info));
        }
        else {
            MEMORY_BASIC_INFORMATION meminfo = {};
            c_memory_region<vad_info> vinfo;
            if (!Hooks::VirtualQueryImpl_(reinterpret_cast<uintptr_t>(lpAddress), &vinfo))
                return 0;

            ZeroMemory(&meminfo, sizeof(meminfo));

            auto found_vad = vinfo.get_object();
            auto rangeStart = vinfo.get_region_start();
            auto rangeEnd = vinfo.get_region_end();
            auto size = rangeEnd - rangeStart + 1;
            meminfo.BaseAddress = reinterpret_cast<PVOID>(rangeStart);
            meminfo.AllocationBase = reinterpret_cast<PVOID>(rangeStart);
            meminfo.AllocationProtect = found_vad.get_protection();
            meminfo.RegionSize = size;
            meminfo.State = found_vad.get_state();
            meminfo.Protect = found_vad.get_protection();
            meminfo.Type = found_vad.get_type();
            meminfo.PartitionId = 0;
            memcpy(lpBuffer, &meminfo, sizeof(meminfo));
            return sizeof(meminfo);
        }
        return sizeof(info);

        // Sleep(500);

        /*VirtualQueryEx(hProcess, lpAddress, &meminfo2, dwLength);
        printf("----------------------------------------------\n");
        printf("BaseAddress: 0x%p -> 0x%p\n", meminfo2.BaseAddress, meminfo.BaseAddress);
        printf("AllocationBase: 0x%p -> 0x%p\n", meminfo2.AllocationBase, meminfo.AllocationBase);
        printf("AllocationProtect: 0x%p -> 0x%p\n", meminfo2.AllocationProtect, meminfo.AllocationProtect);
        printf("RegionSize: 0x%p -> 0x%p\n", meminfo2.RegionSize, meminfo.RegionSize);
        printf("State: 0x%p -> 0x%p\n", meminfo2.State, meminfo.State);
        printf("Protect: 0x%p -> 0x%p\n", meminfo2.Protect, meminfo.Protect);
        printf("Type: 0x%p -> 0x%p\n", meminfo2.Type, meminfo.Type);
        printf("PartitionId: 0x%p -> 0x%p\n", meminfo2.PartitionId, meminfo.PartitionId);
        printf("----------------------------------------------\n");#1#

        return sizeof(meminfo);*/
    }
} // namespace Hooks
