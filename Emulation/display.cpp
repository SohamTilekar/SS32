#include <bitset>
#include <chrono>
#include <thread>
#include <windows.h>

const int WIDTH = 256;  // Width of the image
const int HEIGHT = 256; // Height of the image

void DrawImage(HDC hdc, HBITMAP hBitmap, std::bitset<24> **videoData) {
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = WIDTH;
    bmi.bmiHeader.biHeight = -HEIGHT; // negative to indicate top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    std::vector<uint8_t> buffer(WIDTH * HEIGHT * 3);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            std::bitset<24> pixel = videoData[y][x];
            int index = (y * WIDTH + x) * 3;
            buffer[index + 2] = (pixel.to_ulong() >> 16) & 0xFF; // Red
            buffer[index + 1] = (pixel.to_ulong() >> 8) & 0xFF;  // Green
            buffer[index] = pixel.to_ulong() & 0xFF;             // Blue
        }
    }

    SetDIBitsToDevice(hdc, 0, 0, WIDTH, HEIGHT, 0, 0, 0, HEIGHT, buffer.data(),
                      &bmi, DIB_RGB_COLORS);
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
            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);
            DrawImage(hdc, hBitmap, videoData);
            DeleteObject(hBitmap);
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
        std::this_thread::yield();
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

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Image Display", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
        CW_USEDEFAULT, WIDTH, HEIGHT, NULL, NULL, hInstance, NULL);

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
