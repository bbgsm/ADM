#pragma once
// 游戏版本: v3.0.80.28

/*** r5apex.exe 偏移  ***/
#define OFF_GAME_STATE 0x1EC1FC0    // 对局是否进行 bool
#define OFF_ENTITY_LIST 0x1F61048  // 对象数组(玩家，物品) 大小10000 * 32
#define OFF_LOCAL_PLAYER 0x24342B8  // 操作玩家指针
#define OFF_NAME_LIST 0xD425FE0     // 名称字典指针
#define OFF_MATRIX1 0x2429450       // 4x4矩阵 float[16]
#define OFF_LEVEL_NAME 0x18356C4     // 对局名称
/*** r5apex.exe 偏移 ***/

/*** 玩家，物品 偏移 ***/
#define OFF_VISIBLE_TIME 0x19A0     // 最后可见时间 float(一直可见数值一直会增大)
#define OFF_ITEM_ID 0x1568          // 物品id int
#define OFF_BONES 0xDF8             // 骨骼指针
#define OFF_STUDIO_HDR 0x1000
#define OFF_INDEX_IN_NAMELIST 0x38  // 名称下标
#define OFF_TEAM 0x0338             // 团队id int
#define OFF_SHIELD 0x01a0           // 盾  int
#define OFF_MAX_SHIELD 0x01A4        // 盾最大值 int
#define OFF_ARMOR_TYPE  0x46C4      // 盾类型
#define OFF_NAME 0x0481             // 对象名称 string
#define OFF_ORIGIN 0x017C           // 绝对坐标 xyz float
#define OFF_HEALTH 0x0328           // 血量   int
#define OFF_LIFE_STATE 0x0690       // 存活状态 int

#define OFF_VIEW_ANGLES 0x2520      // 视角水平垂直角度 xy float
#define OFF_VIEW_ANGLES1 0x2230
#define OFF_VIEW_ANGLES2 0x3954
#define OFF_VIEW_ANGLES3 0x36E4
#define OFF_VIEW_ANGLES4 0x327C
#define OFF_VIEW_ANGLES5 0x2AD0
#define OFF_VIEW_ANGLES6 0x1EEF
#define OFF_VIEW_ANGLES7 0x2514
/*** 玩家，物品 偏移 ***/
