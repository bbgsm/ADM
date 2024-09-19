#pragma once

#include "MemoryToolsBase.h"
#include "Type.h"
#include "Vector3D.hpp"
#include "VectorRect.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// 读取骨骼坐标
Vector3D readBonePosition(MemoryToolsBase *mem, Addr player, Vector3D &origin, int id);

// 读取玩家名称
void getName(MemoryToolsBase *mem, uint64_t baseAddr, ulong index, char *name);

// 世界坐标转屏幕坐标
bool worldToScreen(const Vector3D &from, const float *matrix, float screenWidth,
                   float screenHeight, VectorRect &screen);

// 两个3d坐标距离
float computeDistance(Vector3D a, Vector3D b);

// 坐标旋转转换
void rotate(float x1, float y1, Vector2D &mapPosition, float angle);

// 移除非utf8字符
void removeInvalidUTF8(char *str);

// 清空缓存
void gameClear();