#include "ColorHexagon.h"
#include <wingdi.h>
#include <tchar.h>
#include <stdio.h>


#define RETRIEVE_USER_DATA(hwnd) ((InternalColorHexagonState *)GetWindowLongPtr(hwnd, GWLP_USERDATA))


// For GradientFill
#pragma comment(lib, "Msimg32")

#if 0
static const RGBTRIPLE RedColor   = { 0xFF, 0x00, 0x00 };
static const RGBTRIPLE GreenColor = { 0x00, 0xFF, 0x00 };
static const RGBTRIPLE BlueColor  = { 0x00, 0x00, 0xFF };

static const RGBTRIPLE CyanColor    = { 0x00, 0xFF, 0xFF };
static const RGBTRIPLE MagentaColor = { 0xFF, 0x00, 0xFF };
static const RGBTRIPLE YellowColor  = { 0xFF, 0xFF, 0x00 };

static const RGBTRIPLE WhiteColor = { 0xFF, 0xFF, 0xFF };
static const RGBTRIPLE BlackColor = { 0x00, 0x00, 0x00 };
#endif

struct InternalColorHexagonState
{
    RGBTRIPLE color = { 0xFF, 0xFF, 0xFF }; // Default to white

    TRIVERTEX vertex[7] = {};

    static const GRADIENT_TRIANGLE glTriangle[6];

    void setSize(int width, int height)
    {
        const float sqrt3_over_2 = 1.7320508075688772f / 2.0f;
        LONG height_2 = height / 2;
        LONG width_2 = width / 2;
        LONG tri_width = width_2;
        LONG tri_width_2 = tri_width / 2;
        LONG tri_height = (LONG)(sqrt3_over_2 * tri_width);

        tri_height = min(tri_height, height_2);
        tri_width = (LONG)(tri_height / sqrt3_over_2);
        tri_width_2 = tri_width / 2;

        // Center it!
        vertex[0] = { .x = width_2 + tri_width,   .y = height_2,              .Red = 0xFFFF, .Green = 0x0000, .Blue = 0x0000, .Alpha = 0x0000 };
        vertex[1] = { .x = width_2 + tri_width_2, .y = height_2 - tri_height, .Red = 0xFFFF, .Green = 0xFFFF, .Blue = 0x0000, .Alpha = 0x0000 };
        vertex[2] = { .x = width_2 - tri_width_2, .y = height_2 - tri_height, .Red = 0x0000, .Green = 0xFFFF, .Blue = 0x0000, .Alpha = 0x0000 };
        vertex[3] = { .x = width_2 - tri_width,   .y = height_2,              .Red = 0x0000, .Green = 0xFFFF, .Blue = 0xFFFF, .Alpha = 0x0000 };
        vertex[4] = { .x = width_2 - tri_width_2, .y = height_2 + tri_height, .Red = 0x0000, .Green = 0x0000, .Blue = 0xFFFF, .Alpha = 0x0000 };
        vertex[5] = { .x = width_2 + tri_width_2, .y = height_2 + tri_height, .Red = 0xFFFF, .Green = 0x0000, .Blue = 0xFFFF, .Alpha = 0x0000 };
        vertex[6] = { .x = width_2,               .y = height_2,              .Red = 0xFFFF, .Green = 0xFFFF, .Blue = 0xFFFF, .Alpha = 0x0000 };
    }

    PTRIVERTEX vertexData() const
    {
        return (PTRIVERTEX)&vertex;
    }

    static PVOID triangleData()
    {
        return (PVOID)glTriangle;
    }

};

const GRADIENT_TRIANGLE InternalColorHexagonState::glTriangle[6] = {
    { 0, 1, 6 },
    { 1, 2, 6 },
    { 2, 3, 6 },
    { 3, 4, 6 },
    { 4, 5, 6 },
    { 5, 0, 6 }
};

const LPCWSTR ColorHexagonClassName = _TEXT("ColorHexagon");

static void OnSize(HWND hwnd, UINT state, int width, int height)
{
    // Debug printing the size change
    {
        _TCHAR buffer[64];

        _sntprintf_s(buffer,
                     _countof(buffer),
                     _TRUNCATE,
                     _TEXT("%s: OnSize - Width: %d, Height: %d\n"), ColorHexagonClassName, width, height);
        OutputDebugString(buffer);
    }

    InternalColorHexagonState *pState = RETRIEVE_USER_DATA(hwnd);

    if ( pState )
    {
        pState->setSize(width, height);
        InvalidateRect(hwnd, nullptr, TRUE); // We don't draw here.  We just change the size and then invalidate the window to force a repaint.
    }
}

static LRESULT CALLBACK ColorHexagonWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch ( message )
    {
    case WM_CREATE:
        // Debug Print
        {
            _TCHAR buffer[64];

            _sntprintf_s(buffer,
                         _countof(buffer),
                         _TRUNCATE,
                         _TEXT("%s: WM_CREATE\n"), ColorHexagonClassName);
            OutputDebugString(buffer);
        }
        // Store any widget state needed here (i.e. SetWindowLongPtr for user data) if necessary
        {
            InternalColorHexagonState *pState = new InternalColorHexagonState();

            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pState);
        }
        break;
    case WM_COMMAND:
        {
            _TCHAR buffer[64];

            _sntprintf_s(buffer,
                         _countof(buffer),
                         _TRUNCATE,
                         _TEXT("%s: WM_COMMAND\n"), ColorHexagonClassName);
            OutputDebugString(buffer);
        }
        break;
    case WM_SIZE:
        {
            int width = LOWORD(lParam);  // Macro to get the low-order word.
            int height = HIWORD(lParam); // Macro to get the high-order word.

            // Respond to the message:
            OnSize(hWnd, (UINT)wParam, width, height);
        }
        break;
    case WM_PAINT:
        {
            _TCHAR buffer[64];

            _sntprintf_s(buffer,
                         _countof(buffer),
                         _TRUNCATE,
                         _TEXT("%s: WM_PAINT\n"), ColorHexagonClassName);
            OutputDebugString(buffer);
        }
        {
            InternalColorHexagonState *pState = RETRIEVE_USER_DATA(hWnd);
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            if ( pState )
            {
                // Use the state to draw the hexagon
                GradientFill(hdc, pState->vertexData(), 7, pState->triangleData(), 6, GRADIENT_FILL_TRIANGLE);
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case CHM_SETCOLOR:
    {
        InternalColorHexagonState *pState = RETRIEVE_USER_DATA(hWnd);

        if ( pState )
        {
            pState->color = *(PRGBTRIPLE)lParam;
            InvalidateRect(hWnd, nullptr, TRUE); // Force a repaint with the new color
        }
    }
    case CHM_GETCOLOR:
    {
        InternalColorHexagonState *pState = RETRIEVE_USER_DATA(hWnd);

        if ( pState )
        {
            *(PRGBTRIPLE)lParam = pState->color;
        }
    }
    case WM_DESTROY:
        {
            _TCHAR buffer[64];

            _sntprintf_s(buffer,
                         _countof(buffer),
                         _TRUNCATE,
                         _TEXT("%s: WM_DESTROY\n"), ColorHexagonClassName);
            OutputDebugString(buffer);
        }
        {
            InternalColorHexagonState *pState = (InternalColorHexagonState *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

            // It's OK to delete nullptr, so we don't need to check if pState is nullptr before deleting it.
            delete pState;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

ATOM ColorHexagonRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style       = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = ColorHexagonWndProc;
    wcex.hInstance   = hInstance;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszClassName = ColorHexagonClassName;

    return RegisterClassEx(&wcex);
}

BOOL ColorHexagonUnregisterClass(HINSTANCE hInstance)
{
    return UnregisterClass(ColorHexagonClassName, hInstance);
}

void ColorHexagonSetColor(HWND hwnd, RGBTRIPLE color)
{
    SendMessage(hwnd, CHM_SETCOLOR, 0, (LPARAM)&color);
}

RGBTRIPLE ColorHexagonGetColor(HWND hwnd)
{
    RGBTRIPLE color;

    SendMessage(hwnd, CHM_GETCOLOR, 0, (LPARAM)&color);
    return color;
}
