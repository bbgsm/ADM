
#include "imgui.h"
#include "Render.h"

#include <string>

Render render;

void DrawBox(ImColor color, float hp, int d, float x, float y, float w, float h,
             float line_w) {
    ImDrawList *draw = ImGui::GetForegroundDrawList();
    if (x <= 0 || x > render.screenWidth || y <= 0 || y > render.screenHeight) {
        return;
    }

    float ww = w > 100 ? w : 100;
    ImVec2 progressBarSize( ww, 10);
    ImVec2 progressBarPosition{x - progressBarSize.x / 2, y - 12};
    x = x - (w / 2.0f);

    draw->AddRectFilled(progressBarPosition,
                        ImVec2(progressBarPosition.x + progressBarSize.x, progressBarPosition.y + progressBarSize.y),
                        IM_COL32(200, 200, 200, 255)); // 灰色背景
    float progress = hp / 100.0F;
    float progressWidth = progressBarSize.x * progress;
    ImVec2 progressStart = progressBarPosition;
    ImVec2 progressEnd = ImVec2(progressStart.x + progressWidth, progressBarPosition.y + progressBarSize.y);
    draw->AddRectFilled(progressStart, progressEnd, IM_COL32(0, 255, 0, 255)); // 绿色填充
    draw->AddRect(progressBarPosition,
                  ImVec2(progressBarPosition.x + progressBarSize.x, progressBarPosition.y + progressBarSize.y),
                  IM_COL32(0, 0, 0, 255), 0, 15); // 黑色边框
    draw->AddLine(ImVec2(x, y), ImVec2(x + w, y), color, line_w);
    draw->AddLine(ImVec2(x, y), ImVec2(x, y + h), color, line_w);
    draw->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + h), color, line_w);
    draw->AddLine(ImVec2(x, y + h), ImVec2(x + w, y + h), color, line_w);

    std::string text = std::to_string(d) + "M w: " + std::to_string(w) + "h: " + std::to_string(h);
    std::string nameText = "测试";
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    ImVec2 nameTextSize = ImGui::CalcTextSize(nameText.c_str());
    draw->AddText(ImVec2(x + w / 2 - (textSize.x / 2), y + h + 2), IM_COL32(255, 255, 255, 255), text.c_str());

}

void surface() {
    float x = 1000;
    float y = 500;
    float w = 100;
    float h = 100;
    int d = 100;
    int hp = 100;
    float line = 1;
    while (true) {
        render.drawBegin();

        ImDrawList *draw = ImGui::GetForegroundDrawList();

        std::string PerformanceString = "Render FPS: " + std::to_string(static_cast<int>(ImGui::GetIO().Framerate));
        draw->AddText(ImVec2(10, 50), IM_COL32(255, 255, 255, 255), PerformanceString.c_str());

        std::string count = "人数:100";
        ImVec2 textSize = ImGui::CalcTextSize(count.c_str());
        draw->AddText(render.chineseFont, 20, ImVec2(render.screenWidth / 2 - textSize.x / 2, 100),
                         IM_COL32(255, 255, 255, 255),
                         count.c_str());
        ImU32 color1 = IM_COL32(170, 02, 255, 50);

        draw->AddRectFilled(ImVec2(render.screenWidth / 2 - textSize.x / 2 - 5, 95),
                               ImVec2(render.screenWidth / 2 + textSize.x / 2 + 5, 130), color1);
        draw->AddCircleFilled(ImVec2(200, 200), 10, color1);

        ImGui::SetNextWindowSize(ImVec2(400, 430), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(render.screenWidth - 410, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("testGui");
        ImGui::Text("AimAssist Settings");

        ImGui::Text("Sticky Aim:");
        ImGui::SameLine();
        ImGui::Text("测试");

        ImGui::Text("Glow Settings");

        ImGui::SliderFloat("#x", &x, 0.0f, (float) render.screenWidth, "%.2f");
        ImGui::SliderFloat("#y", &y, 0.0f, (float) render.screenHeight, "%.2f");
        ImGui::SliderFloat("#w", &w, 0.0f, 500.0f, "%.2f");
        ImGui::SliderFloat("#h", &h, 0.0f, 500.0f, "%.2f");
        ImGui::SliderInt("#d", &d, 0, 1000);
        ImGui::SliderInt("#hp", &hp, 0, 100);
        ImGui::SliderFloat("#line", &line, 0.0f, 10.0f, "%.1f");

        float fbx = x;
        float fby = y;

        ImU32 color = IM_COL32(255, 0, 0, 255);
        DrawBox(color, hp, d, fbx, fby - h, w, h, line);

        render.drawEnd();
    }
}


int main() {
    render.initImGui(L"testGui");
    surface();
    render.destroyImGui();
}