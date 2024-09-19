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
    int screenHeight = 1080;
    ImFont *chineseFont = nullptr;

public:
    void initImGui(LPCWSTR lpWindowName);

    void destroyImGui();

    void drawBegin();

    void drawEnd();
};


#endif //ADM_RENDER_H
