#include <iostream>
#include <map>
// #define DEBUG_INFO
#include <random>
#include "DirectMemoryTools.h"
#include "DmaMemoryTools.h"
#include "DumpMemoryTools.h"
#include "EventLoop.h"
#include "Game.h"
#include "KmboxNet.h"
#include "MemoryToolsBase.h"
#include "Render.h"
#include "Vector2D.hpp"
#include "Vector3D.hpp"
#include "VectorRect.hpp"
#include "WebSocketServer.h"
#include "hmutex.h"
#include "ini.h"
#include "offsets.h"

MemoryToolsBase *mem;
Render render;
struct EntityAddr {
    Addr addr = 0;
    Addr _addr1 = 0;
    Addr _addr2 = 0;
    Addr _addr3 = 0;
};
// 对象结构体
struct OObject {
    // 距离(米)
    int distance = 0;
    // 血量
    int health = 0;
    // 护盾值/最大护盾值
    int shieldHealth[2] = {0};
    // 名称下标
    int nameIndex = 0;
    // 团队id
    int teamId = -1;
    // 存活状态
    int lifeState = 0;
    // 物品id
    int itemId = 0;
    // 最后可见时间
    float lastVisTime = 0;
    // 是否是玩家
    bool isPlayer = false;
    // 视角旋转
    Vector2D viewAngles;
    // 屏幕坐标
    VectorRect screenPosition;
    // 对象3d坐标
    Vector3D playerPosition;
    // Head position
    Vector3D headPosition;
    // 对象地址
    Addr addr = 0;
    // 名称
    char name[32] = {0};
};

// 全局运行标记
bool isRun = true;
// kmBox是否初始化
bool kmBox = false;
// 距离缩放比例
float distanceScal = 0.025;
// 最大玩家数量
const int maxPlayer = 100;
std::map<Addr, float> lastVisTimeMap;

float fx = 0;
float fy = 0;
float line = 1;
float range = 300;
float matrix[16];

/*** 玩家、物资 ***/
OObject *mapObjectCache = new OObject[1100];
int mapObjectCacheCount = 0;
/*** 玩家、物资 ***/


float vx = 0;
float vy = 0;

// localPlayer 3d 坐标
Vector3D localPlayerPosition;
// 最大物品数
int maxObjectCount = 10000;
int maxDisplayObjectCount = 1000;
// 物品列表读取起始下标
int beginObjectIndex = maxPlayer;

/*** 物资***/
OObject *swapObjects = new OObject[1000];
int objectCount = 0;
/*** 物资***/

EntityAddr *entityAddrs = new EntityAddr[10000];
std::map<Addr, Vector3D> objectsMap;
std::mutex entityMutex;
std::mutex objMutex;
std::mutex webMutex;

float screenCenterX = 0;
float screenCenterY = 0;
int aimBotX = 0;
int aimBotY = 0;
int aimDis = 999999999;
float aimRandomX = 0;
float aimRandomY = 0;
mlong aimTime = 0;
Addr aimAddr = 0;
float aimBotSpeed = 5;
// 自瞄随机部分刷新间隔(毫秒)
mlong aimRandomRefreshDelay = 500;
// 记录上一次循环玩家位置
Vector3D lastPosition;
Vector3D lastHeadPosition;
// 坐标预测
Vector3D positionPrediction;
Vector3D headPositionPrediction;

// 最后预测时间
mlong lastPredictionTime = 0;
// 自瞄预测间隔时间(ms)
int predictionIntervalTime = 150;
// 开启自瞄预测
bool aimPrediction = true;
// 增加随机瞄准
bool aimRandom = true;
// 辅助瞄准
bool aimBot = true;

// 是否开镜
bool isAim = false;

mINI::INIStructure ini;
mINI::INIFile *iniFile;

std::map<int, std::string> mapNames = {
{236, "3倍镜"},  {201, "紫头"},   {202, "金头"},   {195, "医疗箱"},   {280, "涡轮"},         {240, "10倍镜"},
{194, "凤凰"},   {197, "大电"},   {224, "蓝包"},   {225, "紫包"},     {226, "金包"},         {229, "手雷"},
{228, "铝热剂"}, {230, "电弧星"}, {329, "升级包"}, {270, "枪托(紫)"}, {258, "能量弹夹(紫)"}, {259, "能量弹夹(金)"},
{49, "R99"},     {19, "专注"},    {132, "波赛克"}, {1, "克雷贝尔"},   {227, "手刀"},
};

// 重置自瞄参数
void resetAimBot() {
    aimDis = 999999999;
    aimAddr = 0;
    aimRandomY = 0;
    aimRandomX = 0;
    lastPredictionTime = 0;
    lastPosition = {0, 0, 0};
    positionPrediction = {0, 0, 0};
}

int generateRandomNumber(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution dis(min, max);
    return dis(gen);
}


// 获取系统时间(ms)
mlong getCurrentTime() {
    timeval tv{};
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void drawObject(ImColor color, const OObject &object) {
    float x = object.screenPosition.x;
    float y = object.screenPosition.y;
    float w = object.screenPosition.w;
    float h = object.screenPosition.h;

    if (x <= 0 || x > render.screenWidth || y <= 0 || y > render.screenHeight) {
        return;
    }

    ImDrawList *draw = ImGui::GetForegroundDrawList();
    std::string nameText = object.name;
    nameText += "(" + std::to_string(object.distance) + ")";
    ImVec2 textSize = ImGui::CalcTextSize(nameText.c_str());
    draw->AddText(ImVec2(x + w / 2 - (textSize.x / 2), y + h + 2), color, nameText.c_str());
}

void drawPlayer(ImColor color, const OObject &player, float line_w) {
    float x = player.screenPosition.x;
    float y = player.screenPosition.y;
    float w = player.screenPosition.w;
    float h = player.screenPosition.h;

    if (x <= 0 || x > render.screenWidth || y <= 0 || y > render.screenHeight) {
        return;
    }

    ImDrawList *draw = ImGui::GetForegroundDrawList();
    float ww = (std::max)(w, 100.0F);
    ImVec2 progressBarSize(ww, 10);
    ImVec2 progressBarPosition{x - progressBarSize.x / 2, y - 12};

    // 绘制血条背景
    ImVec2 barEndPos{progressBarPosition.x + progressBarSize.x, progressBarPosition.y + progressBarSize.y};
    draw->AddRectFilled(progressBarPosition, barEndPos, IM_COL32(200, 200, 200, 255)); // 灰色背景

    // 计算血条填充部分
    float progressWidth = progressBarSize.x * (player.health / 100.0f);
    ImVec2 progressEnd{progressBarPosition.x + progressWidth, barEndPos.y};
    draw->AddRectFilled(progressBarPosition, progressEnd, IM_COL32(0, 255, 0, 255)); // 绿色填充

    // 绘制血条边框
    draw->AddRect(progressBarPosition, barEndPos, IM_COL32(0, 0, 0, 255), 0, 15); // 黑色边框

    // 调整x坐标以便于绘制矩形框
    float left = x - (w / 2.0f);
    float right = left + w;
    float bottom = y + h;

    // 绘制矩形框
    draw->AddLine(ImVec2(left, y), ImVec2(right, y), color, line_w);
    draw->AddLine(ImVec2(left, y), ImVec2(left, bottom), color, line_w);
    draw->AddLine(ImVec2(right, y), ImVec2(right, bottom), color, line_w);
    draw->AddLine(ImVec2(left, bottom), ImVec2(right, bottom), color, line_w);

    // 准备显示的文本
    std::string text = std::to_string(player.distance) + "M team:(" + std::to_string(player.teamId) + ")";
    std::string nameText = player.name;

    // 计算文本尺寸并绘制
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    ImVec2 nameTextSize = ImGui::CalcTextSize(nameText.c_str());

    draw->AddText(ImVec2(left + (w - textSize.x) / 2, bottom + 2), IM_COL32(255, 255, 255, 255), text.c_str());
    draw->AddText(ImVec2(left + (w - nameTextSize.x) / 2, y - 25), IM_COL32(255, 255, 255, 255), nameText.c_str());
}

/**
 * 对象地址数组读取线程
 */
void entityAddrsRead() {
    const Addr baseAddr = mem->getBaseAddr();
    Addr entityList = baseAddr + OFF_ENTITY_LIST;
    while (isRun) {
        entityMutex.lock();
        mem->readV(entityAddrs, sizeof(EntityAddr) * 10000, entityList);
        entityMutex.unlock();
        hv_delay(1000);
    }
}

/**
 * 物品读取线程
 */
void objTest() {
    auto *objectAddrs = new EntityAddr[10000];
    auto *cacheObjects = new OObject[1000];
    int objectIndex = 0;
    while (isRun) {
        objectIndex = 0;
        int count = maxObjectCount - beginObjectIndex;
        if (count > 0) {
            entityMutex.lock();
            memcpy(objectAddrs, entityAddrs, sizeof(EntityAddr) * count);
            entityMutex.unlock();
            Handle handle = mem->createScatter();
            mem->executeReadScatter(handle);
            mem->closeScatterHandle(handle);
            for (int i = 0; i < count; i++) {
                Addr object = objectAddrs[i].addr;
                if (!MemoryToolsBase::isAddrValid(object)) {
                    continue;
                }
                int itemId = mem->readI(object, OFF_ITEM_ID);
                int isBot = mem->readI(object, OFF_BOT);
                if (!mapNames.contains(itemId) && isBot != 2) {
                    continue;
                }
                cacheObjects[objectIndex].isPlayer = isBot == 2;
                // 物品坐标
                mem->readV(&cacheObjects[objectIndex].playerPosition, sizeof(Vector3D), object, OFF_ORIGIN);
                if (cacheObjects[objectIndex].playerPosition.isZero()) {
                    continue;
                }
                // 超出范围的物品跳过
                float dis =
                computeDistance(localPlayerPosition, cacheObjects[objectIndex].playerPosition) * distanceScal;
                if (dis > 200) {
                    continue;
                }
                std::string str = std::to_string(itemId);
                cacheObjects[objectIndex].itemId = itemId;
                cacheObjects[objectIndex].distance = dis;
                cacheObjects[objectIndex].addr = object;
                if (cacheObjects[objectIndex].isPlayer) {
                    strcpy(cacheObjects[objectIndex].name, "机器人");
                } else {
                    strcpy(cacheObjects[objectIndex].name, mapNames[itemId].c_str());
                }
                // strcpy(cacheObjects[objectIndex].name, longToHex(object).c_str());
                // strcpy(cacheObjects[objectIndex].name, std::to_string(itemId).c_str());
                objectIndex++;
                if (objectIndex >= 1000) {
                    break;
                }
            }
        }
        if (objectIndex > 0) {
            objMutex.lock();
            int s = sizeof(OObject);
            memcpy(swapObjects, cacheObjects, s * objectIndex);
            objectCount = objectIndex;
            objMutex.unlock();
        } else {
            objectCount = 0;
        }
        // 延时10秒
        hv_delay(5000);
    }
    delete[] objectAddrs;
    delete[] cacheObjects;
}

bool readPlayer(OObject &player, OObject &localPlayer, Addr baseAddr, ImU32 *color) {
    mulong entityIndex = player.nameIndex - 1;
    getName(mem, baseAddr, entityIndex, player.name);
    // 团队id校验
    if (player.teamId < 0 || player.teamId > 100) {
        return false;
    }
    if (player.health <= 0) {
        // 人物死亡重置自瞄数据
        if (aimAddr == player.addr) {
            resetAimBot();
        }
        return false;
    }
    // 跳过队友
    if (player.teamId == localPlayer.teamId) {
        return false;
    }
    // 超过范围跳过
    float dis = computeDistance(localPlayer.playerPosition, player.playerPosition) * distanceScal;
    if (dis > range) {
        return false;
    }
    // 读取矩阵
    mem->readV(matrix, sizeof(matrix), baseAddr, OFF_MATRIX1);
    player.distance = dis;
    // 懒得画盾，暂时把盾和血量的总和加起来计算百分比
    // player.health = ((health + shieldHealth) / (maxShieldHealth + maxHealth)) / 100
    player.health = static_cast<int>(
    (static_cast<float>(player.health + player.shieldHealth[0]) / (/* max health */ 100.0F + player.shieldHealth[1])) *
    100.0F);

    /**** 自瞄预测 ****/
    // 延迟150ms左右计算两次坐标距离作为预测值并相加到每一帧上
    mlong time = getCurrentTime();
    if (aimPrediction && aimAddr == player.addr) {
        if (time - lastPredictionTime > predictionIntervalTime /* 150ms*/) {
            if (lastPosition.isZero()) {
                lastPosition = player.playerPosition;
                if (!worldToScreen(player.playerPosition, matrix, render.screenWidth, render.screenHeight,
                                   player.screenPosition)) {
                    return false;
                }
            } else {
                positionPrediction = player.playerPosition - lastPosition;
                lastPosition = player.playerPosition;
            }
            lastPredictionTime = time;
        }
        if (!worldToScreen(positionPrediction + player.playerPosition, matrix, render.screenWidth, render.screenHeight,
                           player.screenPosition)) {
            return false;
        }
    } else {
        if (!worldToScreen(player.playerPosition, matrix, render.screenWidth, render.screenHeight,
                           player.screenPosition)) {
            return false;
        }
    }
    /**** 自瞄预测 ****/

    // 读取头部骨骼坐标
    readBonePosition(mem, player.headPosition, player.playerPosition, player.addr, 0);
    VectorRect headScreenPosition;
    worldToScreen(player.headPosition, matrix, render.screenWidth, render.screenHeight, headScreenPosition);
    player.screenPosition.h = abs(abs(player.screenPosition.y) - abs(headScreenPosition.y));
    player.screenPosition.w = player.screenPosition.h / 2.0f;
    player.screenPosition.x += fx;
    player.screenPosition.y += fy - player.screenPosition.h;
    int d = compute2Distance({player.screenPosition.x, player.screenPosition.y}, {screenCenterX, screenCenterY});

    // 如果盒子初始化完成和开镜
    // 通过人物是否可见还判断自瞄还有点bug，先注释
    if (isAim) {
        if (d < 200 &&
            (player.lastVisTime > lastVisTimeMap[player.addr] /* || (aimAddr == player.addr && lastVisCount < 10)*/)) {
            // 玩家在准心范围200像素内的才进行自瞄操作
            if (d < aimDis || aimAddr == player.addr) {
                /* 自瞄选中 */
                time = getCurrentTime();
                if (aimRandom && (time - aimTime) > aimRandomRefreshDelay) {
                    // Y轴就随机上半身 player.screenPosition.h / 2 就是上半身的范围
                    aimRandomY = static_cast<float>(generateRandomNumber(0, player.screenPosition.h / 2));
                    aimRandomX =
                    static_cast<float>(generateRandomNumber(0, player.screenPosition.w)) - player.screenPosition.w / 2;
                    aimTime = time;
                    logDebug("randomX:%f randomY: %f\n", aimRandomX, aimRandomY);
                }
                aimBotX = player.screenPosition.x - screenCenterX + aimRandomX;
                aimBotY = player.screenPosition.y - screenCenterY + aimRandomY + (player.screenPosition.h / 10);
                aimAddr = player.addr;
                aimDis = d;
            }
        } else if (aimAddr == player.addr) {
            resetAimBot();
        }
    } else {
        resetAimBot();
    }
    if (aimAddr == player.addr) {
        // 辅助瞄准选中颜色
        *color = IM_COL32(0, 255, 0, 255);
    } else if (player.lifeState != 0) {
        // 玩家存活颜色
        *color = IM_COL32(51, 255, 255, 255);
    } else if (player.lastVisTime > lastVisTimeMap[player.addr]) {
        // 玩家可见颜色
        *color = IM_COL32(255, 255, 0, 255);
    } else {
        // 玩家死亡颜色
        *color = IM_COL32(255, 0, 0, 255);
    }
    logDebug("playerAddr: %llX index: %d name:%s\n", player.addr, player.nameIndex, player.name);
    lastVisTimeMap[player.addr] = player.lastVisTime;
    return true;
}
void surface(Addr baseAddr) {
    logInfo("Screen width: %d height %d\n", render.screenWidth, render.screenHeight);
    screenCenterX = render.screenWidth / 2;
    screenCenterY = render.screenHeight / 2;


    auto *players = new OObject[maxPlayer];
    auto *playerAddrs = new EntityAddr[maxPlayer];
    auto *cacheObjects = new OObject[1000];
    int cacheObjectCount = 0;

    int playerCount = 0;
    int playerIndex = 0;

    /******* kmBox初始化 *******/
    char kmBoxIP[16] = "";
    char kmBoxPort[6] = "";
    char kmBoxMac[18] = "";

    std::string ip = ini["kmBox"]["ip"];
    std::string port = ini["kmBox"]["port"];
    std::string uuid = ini["kmBox"]["uuid"];

    if (!ip.empty()) {
        strcpy(kmBoxIP, ip.c_str());
    }
    if (!port.empty()) {
        strcpy(kmBoxPort, port.c_str());
    }
    if (!uuid.empty()) {
        strcpy(kmBoxMac, uuid.c_str());
    }
    if (strlen(kmBoxIP) > 0 && strlen(kmBoxMac) > 0 && strlen(kmBoxPort) > 0) {
        if (kmNet_init(kmBoxIP, kmBoxPort, kmBoxMac) != 0) {
            logInfo("Failed to initialized KmBox\n");
        } else {
            logInfo("Successfully initialized KmBox\n");
            kmNet_monitor(true);
            kmBox = true;
        }
    }
    /******* kmBox初始化 *******/
    while (true) {
        entityMutex.lock();
        memcpy(playerAddrs, entityAddrs, sizeof(EntityAddr) * maxPlayer);
        entityMutex.unlock();

        objMutex.lock();
        if (objectCount > 0) {
            memcpy(cacheObjects, swapObjects, sizeof(OObject) * objectCount);
        }
        cacheObjectCount = objectCount;
        objMutex.unlock();

        if (!render.drawBegin()) {
            continue;
        }
        bool gameState = mem->readB(baseAddr, OFF_GAME_STATE);

        ImDrawList *fpsDraw = ImGui::GetForegroundDrawList();

        std::string PerformanceString = "Render FPS: " + std::to_string(static_cast<int>(ImGui::GetIO().Framerate));
        fpsDraw->AddText(ImVec2(10, 50), IM_COL32(255, 255, 255, 255), PerformanceString.c_str());

        ImGui::SetNextWindowSize(ImVec2(400, 430), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(render.screenWidth - 410, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("ADM");

        ImGui::Checkbox("游戏状态", &gameState);
        ImGui::SliderFloat("框X偏移", &fx, 0.0f, 100.0f, "%.2f");
        ImGui::SliderFloat("框Y偏移", &fy, 0.0f, 100.0f, "%.2f");
        ImGui::SliderFloat("框线宽", &line, 0.0f, 10.0f, "%.1f");
        ImGui::SliderFloat("显示范围(米)", &range, 0.0f, 10000.0f, "%.1f");
        ImGui::SliderFloat("水平视角", &vx, -180, 180, "%.6f");
        ImGui::SliderFloat("垂直视角", &vy, -180, 180, "%.6f");
        ImGui::SliderInt("最大显示物品数", &maxDisplayObjectCount, 0, 1000);
        ImGui::SliderInt("最大读取物品数", &maxObjectCount, 0, 10000);
        ImGui::SliderInt("物品读取开始位置", &beginObjectIndex, maxPlayer, 10000);
        ImGui::Text("辅助瞄准配置:");
        ImGui::Checkbox("辅助瞄准", &aimBot);
        if(aimBot) {
            ImGui::SliderFloat("瞄准速度", &aimBotSpeed, 1.0f, 20.0f, "%.0f");
            ImGui::Checkbox("随机瞄准(上半身范围)", &aimRandom);
            ImGui::Checkbox("开启自瞄预测", &aimPrediction);
            if(aimPrediction) {
                ImGui::SliderInt("预测间隔时间(ms)", &predictionIntervalTime, 0, 1000);
            }
        }
        ImGui::Text("kmBox配置:");
        if (ImGui::InputText("IP", kmBoxIP, 16)) {
            ini["kmBox"]["ip"] = kmBoxIP;
            iniFile->write(ini);
        }
        if (ImGui::InputText("PORT", kmBoxPort, 18)) {
            ini["kmBox"]["port"] = kmBoxPort;
            iniFile->write(ini);
        }
        if (ImGui::InputText("UUID", kmBoxMac, 18)) {
            ini["kmBox"]["uuid"] = kmBoxMac;
            iniFile->write(ini);
        }
        if (ImGui::Button(kmBox ? "已连接" : "连接")) {
            if (kmNet_init(kmBoxIP, kmBoxPort, kmBoxMac) != 0) {
                logInfo("Failed to initialized KmBox\n");
            } else {
                logInfo("Successfully initialized KmBox\n");
                kmBox = true;
            }
        }

        if (ImGui::Button("退出程序")) {
            break;
        }
        ImGui::End();

        if (gameState) {
            Addr localPlayerAddr = mem->readA(baseAddr, OFF_LOCAL_PLAYER);
            logDebug("localPlayerAddr: %llX\n", localPlayerAddr);
            OObject localPlayer;
            localPlayer.isPlayer = true;
            localPlayer.addr = localPlayerAddr;
            Handle handle = mem->createScatter();
            mem->addScatterReadV(handle, &localPlayer.playerPosition, sizeof(Vector3D), localPlayerAddr, OFF_ORIGIN);
            mem->addScatterReadV(handle, &localPlayer.teamId, sizeof(int), localPlayerAddr, OFF_TEAM);
            mem->addScatterReadV(handle, &localPlayer.nameIndex, sizeof(int), localPlayerAddr, OFF_INDEX_IN_NAMELIST);
            mem->addScatterReadV(handle, &localPlayer.itemId, sizeof(int), localPlayerAddr, OFF_ITEM_ID);
            mem->addScatterReadV(handle, &localPlayer.viewAngles, sizeof(Vector2D), localPlayerAddr, OFF_VIEW_ANGLES5);
            mem->addScatterReadV(handle, &isAim, sizeof(bool), localPlayerAddr, OFF_AIM);
            isAim = isAim && kmBox;
            mem->executeReadScatter(handle);
            mem->closeScatterHandle(handle);
            handle = mem->createScatter();
            // Read local player head position
            readBonePosition(mem, localPlayer.headPosition, localPlayer.playerPosition, localPlayer.addr, 0);
            localPlayerPosition = localPlayer.playerPosition;
            vx = localPlayer.viewAngles.x;
            vy = localPlayer.viewAngles.y;
            localPlayer.health = 100;
            players[playerIndex++] = localPlayer;

            for (int i = 0; i < maxPlayer; i++) {
                Addr player = playerAddrs[i].addr;
                if (!MemoryToolsBase::isAddrValid(player)) {
                    continue;
                }
                if (player == localPlayerAddr) {
                    continue;
                }
                // 玩家
                mem->addScatterReadV(handle, &players[playerIndex].teamId, sizeof(int), player, OFF_TEAM);
                mem->addScatterReadV(handle, &players[playerIndex].itemId, sizeof(int), player, OFF_ITEM_ID);
                mem->addScatterReadV(handle, &players[playerIndex].playerPosition, sizeof(Vector3D), player,
                                     OFF_ORIGIN);
                mem->addScatterReadV(handle, &players[playerIndex].viewAngles, sizeof(Vector2D), player,
                                     OFF_VIEW_ANGLES1);
                mem->addScatterReadV(handle, &players[playerIndex].lastVisTime, sizeof(float), player,
                                     OFF_VISIBLE_TIME);
                mem->addScatterReadV(handle, &players[playerIndex].lifeState, sizeof(int), player, OFF_LIFE_STATE);
                mem->addScatterReadV(handle, &players[playerIndex].nameIndex, sizeof(int), player,
                                     OFF_INDEX_IN_NAMELIST);
                mem->addScatterReadV(handle, &players[playerIndex].health, sizeof(int), player, OFF_HEALTH);
                mem->addScatterReadV(handle, players[playerIndex].shieldHealth,
                                     sizeof(players[playerIndex].shieldHealth), player, OFF_SHIELD);
                players[playerIndex].isPlayer = true;
                players[playerIndex].addr = player;
                playerIndex++;
            }
            mem->executeReadScatter(handle);
            mem->closeScatterHandle(handle);
            for (int i = 0; i < playerIndex; i++) {
                OObject &player = players[i];
                ImU32 color = IM_COL32(255, 0, 0, 255);
                if (readPlayer(player, localPlayer, baseAddr, &color)) {
                    drawPlayer(color, player, line);
                    playerCount++;
                }
            }
            // 读取矩阵
            mem->readV(matrix, sizeof(matrix), baseAddr, OFF_MATRIX1);
            handle = mem->createScatter();

            for (int j = 0; j < cacheObjectCount; j++) {
                mem->addScatterReadV(handle, &cacheObjects[j].teamId, sizeof(int), cacheObjects[j].addr, OFF_TEAM);
                mem->addScatterReadV(handle, &cacheObjects[j].itemId, sizeof(int), cacheObjects[j].addr, OFF_ITEM_ID);
                mem->addScatterReadV(handle, &cacheObjects[j].playerPosition, sizeof(Vector3D), cacheObjects[j].addr,
                                     OFF_ORIGIN);
                mem->addScatterReadV(handle, &cacheObjects[j].viewAngles, sizeof(Vector2D), cacheObjects[j].addr,
                                     OFF_VIEW_ANGLES1);
                mem->addScatterReadV(handle, &cacheObjects[j].lastVisTime, sizeof(float), cacheObjects[j].addr,
                                     OFF_VISIBLE_TIME);
                mem->addScatterReadV(handle, &cacheObjects[j].lifeState, sizeof(int), cacheObjects[j].addr,
                                     OFF_LIFE_STATE);
                mem->addScatterReadV(handle, &cacheObjects[j].nameIndex, sizeof(int), cacheObjects[j].addr,
                                     OFF_INDEX_IN_NAMELIST);
                mem->addScatterReadV(handle, &cacheObjects[j].health, sizeof(int), cacheObjects[j].addr, OFF_HEALTH);
                mem->addScatterReadV(handle, cacheObjects[j].shieldHealth, sizeof(cacheObjects[j].shieldHealth),
                                     cacheObjects[j].addr, OFF_SHIELD);
            }
            mem->executeReadScatter(handle);
            mem->closeScatterHandle(handle);
            for (int j = 0; j < cacheObjectCount; j++) {
                if (cacheObjects[j].isPlayer) { // 机器人bot
                    ImU32 color = IM_COL32(255, 0, 0, 255);
                    if (readPlayer(cacheObjects[j], localPlayer, baseAddr, &color)) {
                        drawPlayer(color, cacheObjects[j], line);
                        playerCount++;
                    }
                } else {
                    if (!worldToScreen(cacheObjects[j].playerPosition, matrix, render.screenWidth, render.screenHeight,
                                       cacheObjects[j].screenPosition)) {
                        continue;
                    }
                    float dis = computeDistance(localPlayerPosition, cacheObjects[j].playerPosition) * distanceScal;
                    cacheObjects[j].distance = dis;
                    drawObject(IM_COL32(255, 255, 255, 255), cacheObjects[j]);
                }
            }
            webMutex.lock();
            memcpy(mapObjectCache, players, sizeof(OObject) * playerIndex);
            if (cacheObjectCount > 0) {
                memcpy(&mapObjectCache[playerIndex], cacheObjects, sizeof(OObject) * cacheObjectCount);
            }
            mapObjectCacheCount = playerIndex + cacheObjectCount;
            webMutex.unlock();
            if (playerCount == 0) {
                gameClear();
                lastVisTimeMap.clear();
            }
            playerIndex = 0;
            playerCount = 0;
        }
        render.drawEnd();
    }
    delete[] players;
    delete[] playerAddrs;
    delete[] cacheObjects;
}

using namespace hv;
WebSocketServer server;
WebSocketService wws;
HttpService http;
std::map<WebSocketChannel *, WebSocketChannel *> channels;

/**
 * 网页地图分享功能
 */
class MyContext {
public:
    MyContext() {
        logDebug("MyContext::MyContext()\n");
        timerID = INVALID_TIMER_ID;
    }

    ~MyContext() {
        logDebug("MyContext::~MyContext()\n");
    }

    static int handleMessage(const std::string &msg, enum ws_opcode opcode) {
        logDebug("onmessage(type=%s len=%d): %.*s\n", opcode == WS_OPCODE_TEXT ? "text" : "binary", (int)msg.size(),
                 (int)msg.size(), msg.data());
        return msg.size();
    }

    static void run(WebSocketChannel *channel, OObject *mapObject, int mapObjectCount) {
        Json data;
        for (int i = 0; i < mapObjectCount; i++) {
            OObject &p = mapObject[i];
            Json objectJson;
            if (p.isPlayer) {
                objectJson["x"] = p.playerPosition.x;
                objectJson["y"] = p.playerPosition.y;
                objectJson["z"] = p.playerPosition.z;
                objectJson["teamId"] = p.teamId;
                objectJson["lifeState"] = p.lifeState;
                objectJson["isPlayer"] = p.isPlayer;
                objectJson["health"] = p.health;
                objectJson["viewAnglesV"] = p.viewAngles.x;
                objectJson["viewAnglesH"] = p.viewAngles.y;
                p.name[31] = '\0';
                if (strlen(p.name) <= 0) {
                    continue;
                }
                // 玩家的名字是八仙过海，各显神通，只能在这移除不是utf8的字符，不然Json工具会抛异常
                removeInvalidUTF8(p.name);
                objectJson["name"] = p.name;
            } else {
                objectJson["isPlayer"] = p.isPlayer;
                objectJson["x"] = p.playerPosition.x;
                objectJson["y"] = p.playerPosition.y;
                objectJson["z"] = p.playerPosition.z;
                objectJson["name"] = p.name;
                objectJson["itemId"] = p.itemId;
            }
            //            objectJson["addr"] = p.addr;
            data += {objectJson};
        }
        try {
            if (!data.empty()) {
                std::ostringstream stream;
                stream << data;
                std::string js = stream.str();
                channel->send(js);
            } else {
                channel->send("[]");
            }
        } catch (std::exception &e) {
            logDebug("exception: %s\n", e.what());
        }
    }

    TimerID timerID;
};

void sendWebsocket() {
    auto mapObject = new OObject[1100];
    int mapObjectCount = 0;
    while (isRun) {
        if (mapObjectCacheCount > 0) {
            webMutex.lock();
            memcpy(mapObject, mapObjectCache, sizeof(OObject) * mapObjectCacheCount);
            webMutex.unlock();
        }
        mapObjectCount = mapObjectCacheCount;
        for (auto &channel : channels) {
            if (channel.second->isConnected() && channel.second->isWriteComplete()) {
                MyContext::run(channel.second, mapObject, mapObjectCount);
            }
        }
        hv_delay(100);
    }
    delete[] mapObject;
}

void websocketServer() {
    int port = 6888;
    http.GET("/ping", [](const HttpContextPtr &ctx) { return ctx->send("pong", TEXT_HTML); });
    http.Static("/", "webMap");
    wws.onopen = [](const WebSocketChannelPtr &channel, const HttpRequestPtr &req) {
        logInfo("onopen: GET %s\n", req->Path().c_str());
        auto ctx = channel->newContextPtr<MyContext>();
        channels[channel.get()] = (channel.get());
    };
    wws.onmessage = [](const WebSocketChannelPtr &channel, const std::string &msg) {
        auto ctx = channel->getContextPtr<MyContext>();
        ctx->handleMessage(msg, channel->opcode);
    };
    wws.onclose = [](const WebSocketChannelPtr &channel) {
        channels.erase(channel.get());
        logInfo("onclose\n");
        auto ctx = channel->getContextPtr<MyContext>();
        if (ctx->timerID != INVALID_TIMER_ID) {
            killTimer(ctx->timerID);
            ctx->timerID = INVALID_TIMER_ID;
        }
        // channel->deleteContextPtr();
    };
    server.port = port;
    server.registerHttpService(&http);
    server.registerWebSocketService(&wws);
    server.start();
    logInfo("WebSocket Server initialized\n");
    std::thread sendWebsocketTh(sendWebsocket);
    sendWebsocketTh.detach();
}

// 自瞄线程
void aimBotThread() {
    while (isRun) {
        if (kmBox && aimAddr != 0 && (aimBotX != 0 || aimBotY != 0)) {
            // 曲线公式
            float xx = aimBotSpeed / 10 * sqrt(abs(aimBotX));
            float yy = aimBotSpeed / 10 * sqrt(abs(aimBotY));
            if (xx > 100) {
                xx = 100;
            }
            if (yy > 100) {
                yy = 100;
            }
            xx = xx * (aimBotX > 0 ? 1 : -1);
            yy = yy * (aimBotY > 0 ? 1 : -1);
            // logInfo("aAddr: %llX x:%f y: %f\n", aimAddr, xx, yy);
            // 消抖
            if (abs(xx) < 2) {
                xx = 0;
            }
            // 消抖
            if (abs(yy) < 2) {
                yy = 0;
            }
            if (yy != 0 || xx != 0) {
                kmNet_mouse_move(static_cast<short>(xx), static_cast<short>(yy));
            }
            aimBotX = 0;
            aimBotY = 0;
        }
        hv_delay(10);
    }
}

void plugin() {
    const Addr baseAddr = mem->getBaseAddr();
    logInfo("Pid: %d\n", mem->getProcessPid());
    logInfo("BaseAddr: %llX\n", baseAddr);
    websocketServer();
    // 物品读取线程
    std::thread th(objTest);
    th.detach();

    std::thread eh(entityAddrsRead);
    eh.detach();

    std::thread ah(aimBotThread);
    ah.detach();

    render.initImGui("ADM", 0);
    // 设置显示屏幕下标(多屏幕使用)
    surface(baseAddr);
    render.destroyImGui();
    isRun = false;
}


int main() {
    iniFile = new mINI::INIFile("config.ini");
    if (!iniFile->read(ini)) {
        iniFile->write(ini);
    }
    // 可以直接在游戏电脑上读取内存（不怕封号就逝逝）
    // mem = new DirectMemoryTools();
    // Dma读取内存
    mem = new DmaMemoryTools();
    if (!mem->init("r5apex.exe")) {
        logInfo("Failed to initialized DMA\n");
    } else {
        logInfo("Successfully initialized DMA\n");
    }
    // 读取Dump的内存
    // mem = new DumpMemoryTools();
    // if (!mem->init("C:\\Users\\user\\Desktop\\test\\apex8\\dict.txt")) {
    //     logInfo("Failed to initialized Dump\n");
    // } else {
    //     logInfo("Dump initialized\n");
    // }
    plugin();
    delete iniFile;
    delete mem;
    delete[] entityAddrs;
    delete[] swapObjects;
    delete[] mapObjectCache;
    return 0;
}
