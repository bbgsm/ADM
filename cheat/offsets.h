#pragma once
// 游戏版本: v3.0.82.42

/*** r5apex.exe 偏移  ***/
#define OFF_GAME_STATE 0xAFBD808     // 对局是否进行 int 0x1EC1FC0 0xAFBC60C 0xAFBC610 0xAFBC608
#define OFF_ENTITY_LIST 0x1F62318    // 对象数组(玩家，物品) 大小10000 * 32
#define OFF_LOCAL_PLAYER 0x24354F8   // 操作玩家指针
#define OFF_NAME_LIST 0xD427348      // 名称字典指针
#define OFF_MATRIX1 0x242A690        // 4x4矩阵 float[16]
#define OFF_LEVEL_NAME 0x1836834     // 对局名称
#define OFF_FIRE 0xCF384E8           // (自己)是否开火 bool
#define OFF_FIRE1 0x1753FD4           // 鼠标左键是否点击 bool
/*** r5apex.exe 偏移 ***/

/*** 玩家，物品 偏移 ***/
#define OFF_INDEX_IN_NAMELIST 0x38  // 名称下标
#define OFF_ORIGIN 0x17C            // 绝对坐标 xyz float
#define OFF_SHIELD 0x1A0            // 盾  int
#define OFF_MAX_SHIELD 0x1A4        // 盾最大值 int
#define OFF_HEALTH 0x328            // 血量   int
#define OFF_MAX_HEALTH 0x470        // 最大血量   int

#define OFF_TEAM 0x0338             // 团队id int
#define OFF_NAME 0x0481             // 对象名称 string
#define OFF_LIFE_STATE 0x690        // 存活状态 int
#define OFF_BONES 0xDF8             // 骨骼指针
#define OFF_STUDIO_HDR 0x1000
#define OFF_AIM 0x1BF4              // 是否开镜 bool
#define OFF_ITEM_ID 0x1568          // 物品id int
#define OFF_VISIBLE_TIME 0x19A0     // 最后可见时间 float(一直可见数值一直会增大)
#define OFF_ARMOR_TYPE  0x46C4      // 盾类型

#define OFF_BOT 0x1560             // int == 2 可能等于 机器人

#define OFF_VIEW_ANGLES  0x2520   // 视角水平垂直角度 xy float 0 到 360
#define OFF_VIEW_ANGLES1 0x2230   // -180 到 180
#define OFF_VIEW_ANGLES2 0x39B4   // -180 到 180
#define OFF_VIEW_ANGLES3 0x36E4 + 60 // -180 到 180
#define OFF_VIEW_ANGLES4 0x327C + 28 // -180 到 180
#define OFF_VIEW_ANGLES5 0x2AD0 // -180 到 180
#define OFF_VIEW_ANGLES6 0x1EEF // -180 到 180
#define OFF_VIEW_ANGLES7 0x2510 // -180 到 180
/*** 玩家，物品 偏移 ***/
