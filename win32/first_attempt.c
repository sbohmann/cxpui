#include "../cxpui.h"
#include "../view_list.h"
#include "../core.h"
#include <windows.h>
#include <stdlib.h>

// Forward declarations
void draw_demo_view(struct GraphicsContext context);

// Global state
static struct Window *g_mainWindow = NULL;
static HWND g_hwnd = NULL;

// Window procedure callback
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Helper function to draw a view recursively
void drawView(HDC hdc, struct View *view, RECT *bounds);

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
    int windowWidth = 400;
    int windowHeight = 200;
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    g_hwnd = CreateWindowExW(
        0,
        L"CXPUIWindow",
        L"cxpui Win32 Window",
        WS_OVERLAPPEDWINDOW,
        x, y, windowWidth, windowHeight,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (!g_hwnd) {
        return NULL;
    }

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);

    // Create and initialize the Window struct
    struct Window *window = (struct Window *)allocate(sizeof(struct Window));
    window->base.type = Window;
    window->native_view = g_hwnd;
    window->mainView = NULL;

    g_mainWindow = window;

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
    // Trigger a repaint
    if (g_hwnd) {
        InvalidateRect(g_hwnd, NULL, TRUE);
    }
}

void drawView(HDC hdc, struct View *view, RECT *bounds) {
    if (view == NULL) {
        return;
    }

    if (view->type == CustomView) {
        struct CustomView *customView = (struct CustomView *)view;
        if (customView->paint) {
            struct GraphicsContext gc;
            gc.width = bounds->right - bounds->left;
            gc.height = bounds->bottom - bounds->top;
            gc.native_context = hdc;
            customView->paint(gc);
        }
    } else if (view->type == CompositeView) {
        struct CompositeView *compositeView = (struct CompositeView *)view;

        // Define a context struct to pass both HDC and bounds
        struct PaintContext {
            HDC hdc;
            RECT *bounds;
        };

        struct PaintContext paintCtx = { hdc, bounds };

        // Paint all sub-views
        CompositeView_paint(compositeView, &paintCtx,
            (void (*)(void *, struct View *))drawView);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Fill background with white
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

            // Draw the main view if it exists
            if (g_mainWindow && g_mainWindow->mainView) {
                drawView(hdc, g_mainWindow->mainView, &rect);
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

// Include the demo drawing code from main.c
int get_num_steps(struct GraphicsContext context) {
    int num_x_steps = (int)(context.width / 10);
    int num_y_steps = (int)(context.height / 10);
    return (num_x_steps + num_y_steps) / 2;
}

void draw_demo_view(struct GraphicsContext context) {
    int num_steps = get_num_steps(context);
    for (int index = 0; index <= num_steps; ++index) {
        double x = index * context.width / num_steps;
        double y = index * context.height / num_steps;
        line(context, x, 0, context.width, y);
        line(context, x, context.height, context.width, context.height - y);
        line(context, context.width - x, 0, 0, y);
        line(context, context.width - x, context.height, 0, context.height - y);
    }
    rect(context, context.width / 3.0, context.height / 3.0, context.width / 3.0, context.height / 3.0);
}

// Main entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    struct Window *mainWindow = start();
    struct CompositeView *mainView = CompositeView_create();
    struct CustomView *demoView = CustomView_create();
    demoView->paint = &draw_demo_view;
    ViewList_add(mainView->sub_views, (struct View *)demoView);
    Window_set_main_view(mainWindow, (struct View *)mainView);
    Application_run();
    return 0;
}