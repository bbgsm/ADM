//
// Created by bbgsm on 2024/8/24.
//

#ifndef ADM_RENDER_H
#define ADM_RENDER_H


#include "imgui.h"
#include "windows.h"


class Render {
public:
    int screenWidth = 2560;
    int screenHeight = 1440;
    ImFont *chineseFont = nullptr;

public:
    void initImGui(LPCWSTR lpWindowName);  // 初始化Imgui
    void destroyImGui();                   // 释放imgui资源
    void drawBegin();                      // 会之前调用
    void drawEnd();                        // 绘制结束调用
    void switchMonitor(int monitorIndex);  // 选择显示屏幕下标
};


#endif // ADM_RENDER_H
