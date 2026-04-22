#include "../cxpui.h"
#include "../view_list.h"
#include "../core.h"
#include <windows.h>
#include <stdlib.h>

// Window procedure callback
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Helper function to draw a view recursively
void drawView(HDC hdc, struct View *view);

struct Window *start(void) {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"CXPUIWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc)) {
        return NULL;
    }

    // Create window centered on screen
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 800;
    int windowHeight = 600;
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    HWND hwnd = CreateWindowExW(
        0,
        L"CXPUIWindow",
        L"cxpui Win32 Window",
        WS_OVERLAPPEDWINDOW,
        x, y, windowWidth, windowHeight,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (!hwnd) {
        return NULL;
    }

    // Create and initialize the Window struct
    struct Window *window = (struct Window *)allocate(sizeof(struct Window));
    window->base.type = Window;
    window->base.x = 0;
    window->base.y = 0;
    window->base.width = windowWidth;
    window->base.height = windowHeight;
    window->native_view = hwnd;
    window->mainView = NULL;

    // Store the Window pointer in the HWND for later retrieval
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)window);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return window;
}

void Application_run(void) {
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window_set_main_view(struct Window *window, struct View *view) {
    window->mainView = view;
    view->width = window->base.width;
    view->height = window->base.height;
    // Trigger a repaint
    HWND hwnd = (HWND)window->native_view;
    InvalidateRect(hwnd, NULL, TRUE);
}

void drawView(HDC hdc, struct View *view) {
    if (view == NULL) {
        return;
    }

    if (view->type == CustomView) {
        struct CustomView *customView = (struct CustomView *)view;
        if (customView->paint) {
            struct GraphicsContext gc;
            gc.width = view->width;
            gc.height = view->height;
            gc.native_context = hdc;
            customView->paint(gc);
        }
    } else if (view->type == CompositeView) {
        struct CompositeView *compositeView = (struct CompositeView *)view;

        RECT rect = {
            .left = 0,
            .top = 0,
            .right = view->x + view->width,
            .bottom = view->y + view->height
        };


        FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

        // Paint all sub-views
        CompositeView_paint(compositeView, hdc,
            (void (*)(void *, struct View *))drawView);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Retrieve the Window pointer from HWND
    struct Window *window = (struct Window *)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Draw the main view if it exists
            if (window && window->mainView) {
                drawView(hdc, window->mainView);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SIZE: {
            // Trigger repaint on resize
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void line(struct GraphicsContext gc, double x1, double y1, double x2, double y2) {
    HDC hdc = (HDC)gc.native_context;

    // Create a blue pen (RGB: 0, 0, 255)
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    // In Windows GDI, Y coordinates go down from top
    // We need to flip Y to match AppKit's coordinate system
    int winY1 = (int)(gc.height - y1);
    int winY2 = (int)(gc.height - y2);

    MoveToEx(hdc, (int)x1, winY1, NULL);
    LineTo(hdc, (int)x2, winY2);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void rect(struct GraphicsContext gc, double x, double y, double w, double h) {
    HDC hdc = (HDC)gc.native_context;

    // Create a red brush (RGB: 255, 0, 0)
    HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    // Flip Y coordinate to match AppKit's coordinate system
    int winY = (int)(gc.height - y - h);

    // Use Rectangle function or FillRect
    RECT r;
    r.left = (int)x;
    r.top = winY;
    r.right = (int)(x + w);
    r.bottom = (int)(winY + h);

    FillRect(hdc, &r, brush);

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
}

// Forward declare main from main.c
int main(void);

// Main entry point - just bootstrap and call main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    return main();
}