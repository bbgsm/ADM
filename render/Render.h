//
// Created by bbgsm on 2024/8/24.
//

#ifndef ADM_RENDER_H
#define ADM_RENDER_H

#include <string>
#include "imgui.h"


class Render {
public:
    int screenWidth = 2560;
    int screenHeight = 1440;
    ImFont *chineseFont = nullptr;

public:
    bool initImGui(const std::string &windowName, int monitorIndex);  // 初始化Imgui
    void destroyImGui();                   // 释放imgui资源
    bool drawBegin();                      // 绘制之前调用
    void drawEnd();                        // 绘制结束调用
};


#endif // ADM_RENDER_H
