#include <iostream>
#include <map>

#include "DirectMemoryTools.h"
#include "DmaMemoryTools.h"
#include "DumpMemoryTools.h"
#include "EventLoop.h"
#include "Game.h"
#include "MemoryToolsBase.h"
#include "Render.h"
#include "Vector3D.hpp"
#include "VectorRect.hpp"
#include "WebSocketServer.h"
#include "hmutex.h"
#include "hooks.h"
#include "offsets.h"

MemoryToolsBase *mem;
MemoryToolsBase *mem1;
Render render;

Addr addrs1[] = {0x1FFA5171990, 0x1FFA5171990, 0x1FFA803F4A0, 0x1FFA7474740};
Addr addrs2[] = {0x21F79DD0900, 0x21F79DD5350, 0x221013521E0, 0x220D6660010};
void test() {

    for (int i = 0; i < 0x2FF0; i += 4) {
        bool flag = true;
        int a = mem1->readI(addrs1[0] + i);
        for (int j = 1; j < sizeof(addrs1) / sizeof(addrs1[0]); j++) {
            int b = mem1->readI(addrs1[j] + i);
            if (a != b) {
                flag = false;
                break;
            }
        }
        if(!flag) {
            continue;
        }
        int c = mem->readI(addrs2[0] + i);
        if (c == a) {
            continue;
        }
        flag = true;
        for (int j = 1; j < sizeof(addrs2) / sizeof(addrs2[0]); j++) {
            int d = mem->readI(addrs2[j] + i);
            if (c != d) {
                flag = false;
                break;
            }
        }
        if(flag) {
            printf("off: 0x%llX bot: %d player: %d\n", i, a, c);
        }
    }
}
int main(int argc, char *argv[]) {
    // 可以直接在游戏电脑上读取内存（不怕封号就逝逝）
    // mem = new DirectMemoryTools();
    // Dma读取内存
    // mem = new DmaMemoryTools();
    // if (!mem->init("r5apex.exe")) {
    //     logInfo("Failed to initialized DMA\n");
    // }
    // 读取Dump的内存
    mem = new DumpMemoryTools();
    mem1 = new DumpMemoryTools();
    if (!mem->init("C:\\dumpMem\\apex12\\dict.txt")) {
        logInfo("Failed to initialized Dump\n");
    } else {
        logInfo("Dump initialized\n");
    }
    if (!mem1->init("C:\\dumpMem\\apex11\\dict.txt")) {
        logInfo("Failed to initialized Dump\n");
    } else {
        logInfo("Dump initialized\n");
    }
    // test();
    Addr baseAddr = mem->getBaseAddr();
    Addr localPlayer = mem->readA(baseAddr, OFF_LOCAL_PLAYER);
    printf("localPlayer: %llX\n", localPlayer);

    for (int i = 0; i < 100; ++i) {
        Addr player = mem->readA(baseAddr, OFF_ENTITY_LIST + (i << 5));
        if (!MemoryToolsBase::isAddrValid(player)) {
            continue;
        }
        float a = mem->readF(player, 0x2548);
        int team = mem->readI(player, 0x290);
        int index = mem->readI(player, OFF_INDEX_IN_NAMELIST);
        char name[32] = {0};
        getName(mem, baseAddr, index, name);
        printf("Player: %llX team: %d name: %s\n", player, team, name);
    }

    delete mem;
    return 0;
}
