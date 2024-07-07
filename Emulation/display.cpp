#include <bitset>
#include <chrono>
#include <thread>
#include <windows.h>

const int WIDTH = 256;  // Width of the image
const int HEIGHT = 256; // Height of the image

void DrawImage(HDC hdc, std::bitset<24> **videoData) {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            std::bitset<24> pixel = videoData[y][x];
            COLORREF color = RGB((pixel.to_ulong() >> 16) & 0xFF, // Red
                                 (pixel.to_ulong() >> 8) & 0xFF,  // Green
                                 pixel.to_ulong() & 0xFF          // Blue
            );
            SetPixel(hdc, x, y, color);
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        std::bitset<24> **videoData = reinterpret_cast<std::bitset<24> **>(
            GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (videoData != nullptr) {
            DrawImage(hdc, videoData);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void DisplayThread(HWND hwnd, std::bitset<24> **videoData) {
    SetWindowLongPtr(hwnd, GWLP_USERDATA,
                     reinterpret_cast<LONG_PTR>(videoData));
    while (true) {
        InvalidateRect(hwnd, NULL, FALSE);
        std::this_thread::sleep_for(std::chrono::milliseconds(62)); // ~16 FPS
    }
}

void StartVideoDisplay(std::bitset<24> **videoData) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const wchar_t CLASS_NAME[] = L"ImageDisplayClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0,                   // Optional window styles
                                CLASS_NAME,          // Window class
                                L"Image Display",    // Window text
                                WS_OVERLAPPEDWINDOW, // Window style
                                CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
                                NULL,      // Parent window
                                NULL,      // Menu
                                hInstance, // Instance handle
                                NULL       // Additional application data
    );

    if (hwnd == NULL) {
        return;
    }

    ShowWindow(hwnd, SW_SHOW);

    std::thread displayThread(DisplayThread, hwnd, videoData);
    displayThread.detach();

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
