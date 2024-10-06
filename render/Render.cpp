//
// Created by bbgsm on 2024/8/24.
//
#include "Render.h"
#include "Font.h"
#ifdef _WIN32 // Windows
#include <d3d11.h>
#include <iostream>
#include <vector>
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

HWND hwnd;
WNDCLASSEX wc;

static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;

bool CreateDeviceD3D(HWND hWnd);

void CleanupDeviceD3D();

void CreateRenderTarget();

void CleanupRenderTarget();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Data
static ID3D11Device *g_pd3dDevice = nullptr;
static ID3D11DeviceContext *g_pd3dDeviceContext = nullptr;
static IDXGISwapChain *g_pSwapChain = nullptr;
static ID3D11RenderTargetView *g_mainRenderTargetView = nullptr;

struct MonitorInfo {
    HMONITOR hMonitor;
    RECT rcMonitor;
};

std::vector<MonitorInfo> getMonitors() {
    std::vector<MonitorInfo> monitors;
    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR hMonitor, HDC, LPRECT lprcMonitor, LPARAM dwData) -> BOOL {
        auto* pMonitors = reinterpret_cast<std::vector<MonitorInfo>*>(dwData);
        MONITORINFO mi = { sizeof(mi) };
        if (GetMonitorInfo(hMonitor, &mi)) {
            pMonitors->push_back({ hMonitor, mi.rcMonitor });
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&monitors));
    return monitors;
}


// Helper functions
bool CreateDeviceD3D(HWND hWnd) {
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0,};
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
                                                featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
                                                &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
                                            featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice,
                                            &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (g_pd3dDeviceContext) {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

void CreateRenderTarget() {
    ID3D11Texture2D *pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

bool Render::initImGui(const std::string &windowName, int monitorIndex) {
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);
    // Create application window
    wc = {
            sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L,
            GetModuleHandle(nullptr), nullptr, nullptr, nullptr,
            nullptr, windowName.c_str(), nullptr
    };
    ::RegisterClassEx(&wc);
    hwnd = ::CreateWindowEx(
            WS_EX_LAYERED, wc.lpszClassName, windowName.c_str(),
            WS_POPUP, 0, 0, screenWidth, screenHeight,
            nullptr, nullptr, wc.hInstance, nullptr
    );
    // Set Transparency, WS_EX_LAYERED is required for transparency
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    SetLayeredWindowAttributes(hwnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return false;
    }
    auto monitors = getMonitors();
    if (monitorIndex >= 0 && monitorIndex < monitors.size()) {
        const RECT &rect = monitors[monitorIndex].rcMonitor;
        screenWidth = rect.right - rect.left;
        screenHeight = rect.bottom - rect.top;
        SetWindowPos(hwnd, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                     SWP_SHOWWINDOW);
    } else {
        std::cerr << "Invalid monitor index." << std::endl;
        screenWidth = GetSystemMetrics(SM_CXSCREEN);
        screenHeight = GetSystemMetrics(SM_CYSCREEN);
    }
    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    auto *OPPOSans = new unsigned int[2219752 / 4];
    memcpy(OPPOSans, OPPOSans_H, 2219752);
    // Setup fonts
    chineseFont = io.Fonts->AddFontFromMemoryTTF((void *) OPPOSans, OPPOSans_H_size, 15, nullptr,
                                                 io.Fonts->GetGlyphRangesChineseFull());
    io.FontDefault = chineseFont;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    return true;
}

void Render::destroyImGui() {
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

bool Render::drawBegin() {
    if (hwnd != nullptr) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            break;
    }
    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
        CleanupRenderTarget();
        g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
        g_ResizeWidth = g_ResizeHeight = 0;
        CreateRenderTarget();
    }
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    return true;
}

void Render::drawEnd() {
    // Rendering
    ImGui::Render();
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                                             clear_color.z * clear_color.w, clear_color.w};
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    g_pSwapChain->Present(0, 0); // Present without vsync
}


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    switch (msg) {
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
                return 0;
            g_ResizeWidth = (UINT) LOWORD(lParam); // Queue resize
            g_ResizeHeight = (UINT) HIWORD(lParam);
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
#else // Linux

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <imgui_internal.h>
#include <iostream>
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <GLFW/glfw3native.h>

bool enableClickThrough = false;
GLFWwindow* window;

void delay_ms(int milliseconds)
{
    timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

void setWindowClickThrough(GLFWwindow* window, bool enable)
{
    Display* display = XOpenDisplay(NULL);
    if (!display)
    {
        return;
    }
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    // printf("Window size: %dx%d\n", width, height);
    // 获取原始的 X11 窗口 ID
    Window x11_window = glfwGetX11Window(window);
    XRectangle rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = height;
    if (enable)
    {
        // 设置窗口为点击穿透
        XShapeCombineRectangles(display, x11_window, ShapeInput, 0, 0, &rect, 1, ShapeSubtract, Unsorted);
        // printf("enable\n");
    }
    else
    {
        XShapeCombineRectangles(display, x11_window, ShapeInput, 0, 0, &rect, 1, ShapeSet, Unsorted);
        // printf("disable\n");
    }
    XSync(display,true);
    XCloseDisplay(display);
    delay_ms(1);
}

void window_focus_callback(GLFWwindow* window, int focused) {
    if (focused) {
        setWindowClickThrough(window, false);
    }
}


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

bool Render::initImGui(const std::string &windowName, int monitorIndex){
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return false;
    const char* glsl_version = "#version 130";
    // 设置窗口属性
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    // 配置窗口属性
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // 禁用窗口装饰
    // 获取所有可用的显示器
    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    if (monitorIndex >= count || monitorIndex < 0) {
        monitorIndex = 0;
    }
    // 通过下标获取显示器
    GLFWmonitor* monitor = monitors[monitorIndex];
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    screenWidth = mode->width;
    screenHeight = mode->height;
    window = glfwCreateWindow(mode->width, mode->height, windowName.c_str(), monitor, NULL);
    if (window == nullptr)
        return false;
    // 设置窗口焦点回调
    glfwSetWindowFocusCallback(window, window_focus_callback);
    // Set window on top
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    auto *OPPOSans = new unsigned int[2219752 / 4];
    memcpy(OPPOSans, OPPOSans_H, 2219752);
    // Setup fonts
    chineseFont = io.Fonts->AddFontFromMemoryTTF((void *) OPPOSans, OPPOSans_H_size, 15, nullptr,
                                                 io.Fonts->GetGlyphRangesChineseFull());
    io.FontDefault = chineseFont;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    return true;
}

void Render::destroyImGui(){
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Render::drawBegin(){
    if(glfwWindowShouldClose(window)) {
        return false;
    }
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
    {
        ImGui_ImplGlfw_Sleep(10);
        return false;
    }
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    return true;
}

void Render::drawEnd(){
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        if (enableClickThrough)
        {
            enableClickThrough = false;
            setWindowClickThrough(window,enableClickThrough);
        }
    }
    else
    {
        if (!enableClickThrough)
        {
            enableClickThrough = true;
            setWindowClickThrough(window,enableClickThrough);
        }
    }
    glfwSwapBuffers(window);
}
#endif