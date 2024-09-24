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
void getName(MemoryToolsBase *mem, Addr baseAddr, ulong index, char *name);

// 世界坐标转屏幕坐标
bool worldToScreen(const Vector3D &from, const float *matrix, float screenWidth,
                   float screenHeight, VectorRect &screen);

// 两个3d坐标距离
float computeDistance(Vector3D a, Vector3D b);

// 坐标旋转转换
void rotate(Vector2D a, Vector2D b, Vector2D &mapPosition, float viewAnglesH) ;

// 移除非utf8字符
void removeInvalidUTF8(char *str);

// 清空缓存
void gameClear();