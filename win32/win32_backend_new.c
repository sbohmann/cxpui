/*
 * Win32 (GDI) backend for cxpui — top-left coordinate origin.
 *
 * Lifecycle: unlike GTK4, Win32 lets us create the window in start()
 * before entering the message loop, so the sequence is straightforward:
 *   start() → creates HWND, returns Window*
 *   main.c sets up views, calls Window_set_main_view()
 *   Application_run() → enters the message loop
 *
 * Drawing: line() and rect() receive the HDC via
 * GraphicsContext.native_context, same pattern as the GTK4 backend
 * uses GtkSnapshot*.
 */

#include <windows.h>
#include "../cxpui.h"
#include "../view_list.h"
#include "../core.h"

/* ── Global state ────────────────────────────────────────────────── */

static struct Window *g_mainWindow = NULL;
static HWND           g_hwnd       = NULL;

/* ── Forward declarations ────────────────────────────────────────── */

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static void paint_view(HDC hdc, struct View *view);

/* ── Drawing primitives (GDI) ────────────────────────────────────── */

void line(struct GraphicsContext gc, double x1, double y1, double x2, double y2) {
    HDC hdc = (HDC)gc.native_context;
    HPEN pen    = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));   /* blue */
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    MoveToEx(hdc, (int)x1, (int)y1, NULL);
    LineTo(hdc,   (int)x2, (int)y2);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void rect(struct GraphicsContext gc, double x, double y, double w, double h) {
    HDC hdc = (HDC)gc.native_context;
    HBRUSH brush    = CreateSolidBrush(RGB(255, 0, 0));     /* red */
    RECT r = {
        .left   = (int)x,
        .top    = (int)y,
        .right  = (int)(x + w),
        .bottom = (int)(y + h),
    };
    FillRect(hdc, &r, brush);
    DeleteObject(brush);
}

/* ── View painting ───────────────────────────────────────────────── */

/*
 * Trampoline for CompositeView_paint: signature must match
 *   void (*)(void *context, struct View *view)
 */
static void paint_view_trampoline(void *context, struct View *view) {
    paint_view((HDC)context, view);
}

static void paint_view(HDC hdc, struct View *view) {
    if (!view) return;

    switch (view->type) {
    case CustomView: {
        struct CustomView *cv = (struct CustomView *)view;
        if (cv->paint) {
            struct GraphicsContext gc = {
                .width          = (double)view->width,
                .height         = (double)view->height,
                .native_context = hdc,
            };
            cv->paint(gc);
        }
        break;
    }
    case CompositeView: {
        struct CompositeView *comp = (struct CompositeView *)view;
        CompositeView_paint(comp, hdc, paint_view_trampoline);
        break;
    }
    default:
        break;
    }
}

/* ── Window procedure ────────────────────────────────────────────── */

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        /* White background */
        RECT client;
        GetClientRect(hwnd, &client);
        FillRect(hdc, &client, (HBRUSH)GetStockObject(WHITE_BRUSH));

        if (g_mainWindow && g_mainWindow->mainView) {
            struct View *root = g_mainWindow->mainView;

            /* Propagate actual client size down to the view tree,
             * same as the GTK4 backend does in its snapshot vfunc. */
            root->width  = client.right  - client.left;
            root->height = client.bottom - client.top;

            if (root->type == CompositeView) {
                struct CompositeView *comp = (struct CompositeView *)root;
                size_t n = ViewList_size(comp->sub_views);
                for (size_t i = 0; i < n; i++) {
                    struct View *child = ViewList_get(comp->sub_views, i);
                    child->width  = root->width;
                    child->height = root->height;
                }
            }

            paint_view(hdc, root);
        }

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_SIZE:
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

/* ── Public API (cxpui.h) ────────────────────────────────────────── */

struct Window *start(void) {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance      = GetModuleHandle(NULL);
    wc.lpszClassName  = L"CXPUIWindow";
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc)) {
        fail_with_message("RegisterClassW failed");
    }

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int winW = 800, winH = 600;

    g_hwnd = CreateWindowExW(
        0, L"CXPUIWindow", L"cxpui \x2013 Win32",
        WS_OVERLAPPEDWINDOW,
        (screenW - winW) / 2, (screenH - winH) / 2,
        winW, winH,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!g_hwnd) {
        fail_with_message("CreateWindowExW failed");
    }

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);

    struct Window *w = allocate(sizeof(struct Window));
    *w = (struct Window){0};
    w->base.type   = Window;
    w->base.width  = winW;
    w->base.height = winH;
    w->native_view = g_hwnd;
    w->mainView    = NULL;

    g_mainWindow = w;
    return w;
}

void Application_run(void) {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

struct Window *Window_create(void *native_view) {
    struct Window *w = allocate(sizeof(struct Window));
    *w = (struct Window){0};
    w->base.type   = Window;
    w->native_view = native_view;
    return w;
}

void Window_set_main_view(struct Window *window, struct View *view) {
    window->mainView = view;
    if (g_hwnd) {
        InvalidateRect(g_hwnd, NULL, TRUE);
    }
}

struct NativeView *NativeView_create(void *native_view) {
    struct NativeView *nv = allocate(sizeof(struct NativeView));
    *nv = (struct NativeView){0};
    nv->base.type   = NativeView;
    nv->native_view = native_view;
    return nv;
}

/* ── Entry point ─────────────────────────────────────────────────── */

/* Forward-declare main() from main.c */
int main(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    (void)hInstance; (void)hPrevInstance; (void)lpCmdLine; (void)nCmdShow;
    return main();
}
