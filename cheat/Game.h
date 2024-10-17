#pragma once

#include "MemoryToolsBase.h"
#include "Type.h"
#include "Vector2D.hpp"
#include "Vector3D.hpp"
#include "VectorRect.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// 读取骨骼坐标
void readBonePosition(MemoryToolsBase *mem, Vector3D &headPosition, Vector3D &origin, Addr player, int id);

// 读取玩家名称
void getName(MemoryToolsBase *mem, Addr baseAddr, int index, char *name);

// 世界坐标转屏幕坐标
bool worldToScreen(const Vector3D &from, const float *matrix, float screenWidth, float screenHeight,
                   VectorRect &screen);

// 两个3d坐标距离
float computeDistance(Vector3D a, Vector3D b);

float compute2Distance(Vector2D a, Vector2D b);

// 坐标旋转转换
void rotate(Vector2D a, Vector2D b, Vector2D &mapPosition, float viewAnglesH);

// 移除非utf8字符
void removeInvalidUTF8(char *str);

// 清空缓存
void gameClear();

void sleep_s(int s);

void sleep_ms(int ms);

// 平滑鼠标移动(滤波)
class MouseSmoother {
public:
    MouseSmoother(int windowSize) :
        windowSize(windowSize) {
    }
    Vector2D smoothPosition(Vector2D newPoint) {
        positions.push_back(newPoint);
        if (positions.size() > windowSize) {
            positions.erase(positions.begin());
        }
        return getSmoothedPosition();
    }
    void clear() {
        positions.clear();
    }

    void setWindowsSize(int w) {
        windowSize = w;
    }
private:
    std::vector<Vector2D> positions;
    int windowSize;

    Vector2D getSmoothedPosition() {
        Vector2D smoothed = {0, 0};
        int count = static_cast<int>(positions.size());
        for (const auto &pos : positions) {
            smoothed.x += pos.x;
            smoothed.y += pos.y;
        }
        smoothed.x /= count;
        smoothed.y /= count;
        return smoothed;
    }
};
