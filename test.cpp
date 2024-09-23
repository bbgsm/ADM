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
Render render;

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
    if (!mem->init("C:\\Users\\user\\Desktop\\test\\apex9\\dict.txt")) {
        logInfo("Failed to initialized Dump\n");
    } else {
        logInfo("Dump initialized\n");
    }

    Addr baseAddr = mem->getBaseAddr();
    Addr localPlayer = mem->readA(baseAddr, OFF_LOCAL_PLAYER);
    printf("localPlayer: %llX\n", localPlayer);

    for (int i = 0; i < 100; ++i) {
        Addr player = mem->readA(baseAddr, OFF_ENTITY_LIST + (i << 5));
        if (!MemoryToolsBase::isAddrValid(player)) {
            continue;
        }
        float a = mem->readF(player, 0x2548 );
        int team = mem->readI(player, OFF_TEAM);
        printf("Player: %llX team: %d a: %f\n", player,team, a);
    }

    delete mem;
    return 0;
}
