#include <vector>
#include <windows.h>

struct MonitorInfo {
    HMONITOR hMonitor;
    RECT rect;
};

std::vector<MonitorInfo> monitors;
int currentMonitorIndex = 0;

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT lprcMonitor, LPARAM) {
    monitors.push_back({hMonitor, *lprcMonitor});
    return TRUE;
}

void MoveToMonitor(HWND hwnd, int monitorIndex) {
    if (monitorIndex >= 0 && monitorIndex < monitors.size()) {
        RECT rect = monitors[monitorIndex].rect;
        SetWindowPos(hwnd, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                     SWP_SHOWWINDOW);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int clickCount = 0;
    static DWORD lastClickTime = 0;

    switch (uMsg) {
        case WM_LBUTTONDOWN: {
            DWORD currentTime = GetTickCount();
            if (currentTime - lastClickTime < GetDoubleClickTime()) {
                clickCount++;
            } else {
                clickCount = 1;
            }
            lastClickTime = currentTime;
            if (clickCount == 2) {
                PostQuitMessage(0);
            }
            break;
        }
        case WM_RBUTTONDOWN: {
            currentMonitorIndex = (currentMonitorIndex + 1) % monitors.size();
            MoveToMonitor(hwnd, currentMonitorIndex);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(hdc, &ps.rcPaint, blackBrush);
            DeleteObject(blackBrush);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY: PostQuitMessage(0); break;
        default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// 黑屏工具，用于融合器背景，双击鼠标左键退出
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0);
    const wchar_t CLASS_NAME[] = L"BlackScreenWindow";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassW(&wc);
    HWND hwnd = CreateWindowExW(WS_EX_TOPMOST, CLASS_NAME, L"Black Screen", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                hInstance, nullptr);
    if (hwnd == nullptr) {
        return 0;
    }
    MoveToMonitor(hwnd, currentMonitorIndex);
    ShowWindow(hwnd, nCmdShow);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
