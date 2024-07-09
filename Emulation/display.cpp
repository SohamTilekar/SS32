#include "emulator.hpp"
#include <d2d1.h>
#include <vector>
#include <wincodec.h>

#pragma comment(lib, "d2d1")

ID2D1Factory *pFactory = nullptr;
ID2D1HwndRenderTarget *pRenderTarget = nullptr;

void CreateBitmapFromData(ID2D1Bitmap **ppBitmap, uint8_t *videoData) {
    if (!pRenderTarget)
        return;
    D2D1_BITMAP_PROPERTIES bitmapProperties;
    bitmapProperties.pixelFormat =
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
    bitmapProperties.dpiX = 96.0f;
    bitmapProperties.dpiY = 96.0f;

    D2D1_SIZE_U size = D2D1::SizeU(WIDTH, HEIGHT);

    if (*ppBitmap) {
        (*ppBitmap)->Release();
        *ppBitmap = nullptr;
    }

    pRenderTarget->CreateBitmap(size, videoData, WIDTH * 4, &bitmapProperties,
                                ppBitmap);
}

void DiscardGraphicsResources() {
    if (pRenderTarget)
        pRenderTarget->Release();
}

void InitializeDirect2D(HWND hwnd) {
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);

    RECT rc;
    GetClientRect(hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    pFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hwnd, size), &pRenderTarget);
}

void DrawImage(uint8_t *videoData) {
    if (!pRenderTarget)
        return;

    ID2D1Bitmap *pBitmap = nullptr;
    CreateBitmapFromData(&pBitmap, videoData);

    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
    if (pBitmap) {
        pRenderTarget->DrawBitmap(pBitmap, D2D1::RectF(0, 0, WIDTH, HEIGHT));
        pBitmap->Release();
    }
    pRenderTarget->EndDraw();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        uint8_t *videoData =
            reinterpret_cast<uint8_t *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (videoData != nullptr) {
            DrawImage(videoData);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_SIZE:
        if (pRenderTarget) {
            RECT rc;
            GetClientRect(hwnd, &rc);
            D2D1_SIZE_U size =
                D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
            pRenderTarget->Resize(size);
        }
        return 0;
    case WM_DESTROY:
        DiscardGraphicsResources();
        PostQuitMessage(0);
        exit(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void DisplayThread(HWND hwnd, uint8_t *videoData) {
    SetWindowLongPtr(hwnd, GWLP_USERDATA,
                     reinterpret_cast<LONG_PTR>(videoData));
    while (true) {
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

void StartVideoDisplay(uint8_t *videoData) {
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

    InitializeDirect2D(hwnd);

    std::thread displayThread(DisplayThread, hwnd, videoData);
    displayThread.detach();

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        std::this_thread::yield();
    }

    if (pFactory)
        pFactory->Release();
}
