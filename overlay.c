#include <windows.h>

#define ID_HOTKEY_TOGGLE 1
#define ID_HOTKEY_MODE1  2
#define ID_HOTKEY_MODE2  3

int overlayVisible = 0;
int mode = 0; // 0=dot, 1=line, 2=burst

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_HOTKEY:
            if (wParam == ID_HOTKEY_TOGGLE) {
                overlayVisible = !overlayVisible;
                Beep(overlayVisible ? 1000 : 600, 150);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (wParam == ID_HOTKEY_MODE1) {
                mode = 1;
                Beep(1200, 150);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (wParam == ID_HOTKEY_MODE2) {
                mode = 2;
                Beep(1500, 150);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;

        case WM_PAINT:
            if (overlayVisible) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                RECT rect;
                GetClientRect(hwnd, &rect);

                int cx = rect.right / 2;
                int cy = rect.bottom / 2;

                HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
                HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
                SelectObject(hdc, hPen);
                SelectObject(hdc, hBrush);

                if (mode == 0) {
                    // Dot mode
                    Ellipse(hdc, cx - 5, cy - 5, cx + 5, cy + 5);
                }
                else if (mode == 1) {
                    // Drag line mode
                    MoveToEx(hdc, cx, cy + 40, NULL);
                    LineTo(hdc, cx, cy - 40);
                }
                else if (mode == 2) {
                    // Burst dot mode (smaller dot)
                    Ellipse(hdc, cx - 3, cy - 3, cx + 3, cy + 3);
                }

                DeleteObject(hPen);
                DeleteObject(hBrush);
                EndPaint(hwnd, &ps);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "OverlayWindow";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        wc.lpszClassName, "Overlay",
        WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL
    );

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    ShowWindow(hwnd, nCmdShow);

    // Register hotkeys
    RegisterHotKey(hwnd, ID_HOTKEY_TOGGLE, MOD_NOREPEAT, VK_F1);
    RegisterHotKey(hwnd, ID_HOTKEY_MODE1, MOD_NOREPEAT, VK_F2);
    RegisterHotKey(hwnd, ID_HOTKEY_MODE2, MOD_NOREPEAT, VK_F3);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterHotKey(hwnd, ID_HOTKEY_TOGGLE);
    UnregisterHotKey(hwnd, ID_HOTKEY_MODE1);
    UnregisterHotKey(hwnd, ID_HOTKEY_MODE2);

    return 0;
}
