#include "ColorHexagon.h"
#include <wingdi.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include <cmath>
#include <cassert>
#include <numbers>
#include <algorithm>


#define RETRIEVE_USER_DATA(hwnd) ((InternalColorHexagonState *)GetWindowLongPtr(hwnd, GWLP_USERDATA))


// For GradientFill
#pragma comment(lib, "Msimg32")


static const float SQRT3_OVER_2 = 1.7320508075688772f / 2.0f;
static const float TWO_PI       = 2.0f * std::numbers::pi_v<float>;

static float DotProduct(POINT our_vector, POINT reference_vector)
{
    return our_vector.x * reference_vector.x + our_vector.y * reference_vector.y;
}

struct InternalColorHexagonState
{
    static const GRADIENT_TRIANGLE glTriangle[6];

    UNITHSVTRIPLE color = { .hue = 0.0f, .saturation = 0.0f, .value = 1.0f }; // Default to white
    TRIVERTEX     vertex[7] = { };
    POINT         center = { 0, 0 };
    POINT         red_translation_vector   = { 0, 0 };
    POINT         green_translation_vector = { 0, 0 };
    POINT         blue_translation_vector  = { 0, 0 };
    bool          we_are_tracking_mouse = false;

    void setSize(int width, int height)
    {
        LONG height_2 = height / 2;
        LONG width_2 = width / 2;
        LONG tri_width = width_2;
        LONG tri_width_2 = tri_width / 2;
        LONG tri_height = (LONG)(SQRT3_OVER_2 * tri_width);

        tri_height = min(tri_height, height_2);
        tri_width = (LONG)(tri_height / SQRT3_OVER_2);
        tri_width_2 = tri_width / 2;

        red_translation_vector.x = tri_width;
        red_translation_vector.y = 0;

        green_translation_vector.x = -tri_width_2;
        green_translation_vector.y = -tri_height;

        blue_translation_vector.x = -tri_width_2;
        blue_translation_vector.y =  tri_height;

        center.x = width_2;
        center.y = height_2;

        // Center it!

        // Red
        vertex[0] = { .x = center.x + tri_width,   .y = center.y,              .Red = 0xFFFF, .Green = 0x0000, .Blue = 0x0000, .Alpha = 0x0000 };

        vertex[1] = { .x = center.x + tri_width_2, .y = center.y - tri_height, .Red = 0xFFFF, .Green = 0xFFFF, .Blue = 0x0000, .Alpha = 0x0000 };

        // Green
        vertex[2] = { .x = center.x - tri_width_2, .y = center.y - tri_height, .Red = 0x0000, .Green = 0xFFFF, .Blue = 0x0000, .Alpha = 0x0000 };

        vertex[3] = { .x = center.x - tri_width,   .y = center.y,              .Red = 0x0000, .Green = 0xFFFF, .Blue = 0xFFFF, .Alpha = 0x0000 };

        // Blue
        vertex[4] = { .x = center.x - tri_width_2, .y = center.y + tri_height, .Red = 0x0000, .Green = 0x0000, .Blue = 0xFFFF, .Alpha = 0x0000 };

        vertex[5] = { .x = center.x + tri_width_2, .y = center.y + tri_height, .Red = 0xFFFF, .Green = 0x0000, .Blue = 0xFFFF, .Alpha = 0x0000 };

        // Center (White)
        vertex[6] = { .x = center.x,               .y = center.y,              .Red = 0xFFFF, .Green = 0xFFFF, .Blue = 0xFFFF, .Alpha = 0x0000 };
    }

    POINT colorValueToWidgetXYCoords() const
    {
        UNITRGBTRIPLE rgb_value = UnitHSVTripleToUnitRGBTriple(color);
        POINT         scaled_red_vector = { .x = (LONG)std::round(red_translation_vector.x * rgb_value.red),
                                            .y = (LONG)std::round(red_translation_vector.y * rgb_value.red) };
        POINT         scaled_green_vector = { .x = (LONG)std::roundf(green_translation_vector.x * rgb_value.green),
                                              .y = (LONG)std::round(green_translation_vector.y * rgb_value.green) };
        POINT         scaled_blue_vector = { .x = (LONG)std::round(blue_translation_vector.x * rgb_value.blue),
                                             .y = (LONG)std::round(blue_translation_vector.y * rgb_value.blue) };
        POINT         indicator_location = { .x = scaled_red_vector.x + scaled_green_vector.x + scaled_blue_vector.x,
                                             .y = scaled_red_vector.y + scaled_green_vector.y + scaled_blue_vector.y };
        POINT         indicator_from_center = { .x = center.x + indicator_location.x,
                                                .y = center.y + indicator_location.y };

        return indicator_from_center;
    }

    UNITRGBTRIPLE widgetXYCoordsRGBUnitVectorPercentages(int x, int y) const
    {
        POINT indicator_from_center = { .x = x - center.x, .y = y - center.y };
        float red_contribution   = DotProduct(indicator_from_center, red_translation_vector)   / (float)(red_translation_vector.x * red_translation_vector.x + red_translation_vector.y * red_translation_vector.y);
        float green_contribution = DotProduct(indicator_from_center, green_translation_vector) / (float)(green_translation_vector.x * green_translation_vector.x + green_translation_vector.y * green_translation_vector.y);
        float blue_contribution  = DotProduct(indicator_from_center, blue_translation_vector)  / (float)(blue_translation_vector.x * blue_translation_vector.x + blue_translation_vector.y * blue_translation_vector.y);

        return { std::clamp(red_contribution,   0.0f, 1.0f),
                 std::clamp(green_contribution, 0.0f, 1.0f),
                 std::clamp(blue_contribution,  0.0f, 1.0f) };
    }

    // NOTE: This assumes the input RGB values are calculated as if the HSV value is 1.0. (i.e. full-color)
    UNITRGBTRIPLE rgbValueScaledToCurrentHSVValue(UNITRGBTRIPLE rgb_value) const
    {
        // Scale the RGB values by the current HSV value (i.e. the "value" component of the HSV color)
        rgb_value.red   *= color.value;
        rgb_value.green *= color.value;
        rgb_value.blue  *= color.value;

        return rgb_value;
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

static void DrawColorIndicator(HDC hdc, InternalColorHexagonState *pState)
{
    const LONG radius = 5; // Radius of the indicator circle

    POINT indicator_from_center = pState->colorValueToWidgetXYCoords();
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(WHITE_BRUSH));

    Ellipse(hdc,
            indicator_from_center.x - radius,
            indicator_from_center.y - radius,
            indicator_from_center.x + radius,
            indicator_from_center.y + radius);
    SelectObject(hdc, oldBrush);
}

static void ColorHexagonHandleMouseLButtonDown(HWND hWnd, int xPos, int yPos, InternalColorHexagonState *pState)
{
    // Implementation for handling mouse down event
    pState->we_are_tracking_mouse = true;

    UNITRGBTRIPLE rgbValue = pState->widgetXYCoordsRGBUnitVectorPercentages(xPos, yPos);

    rgbValue = pState->rgbValueScaledToCurrentHSVValue(rgbValue);

    UNITHSVTRIPLE hsvValue = UnitRGBTripleToUnitHSVTriple(rgbValue);

    hsvValue.value = pState->color.value; // Preserve the current value (brightness) component
#if 1
    UNITRGBTRIPLE rgbValueCheck = UnitHSVTripleToUnitRGBTriple(hsvValue);

    assert(rgbValue.red == rgbValueCheck.red);
    assert(rgbValue.green == rgbValueCheck.green);
    assert(rgbValue.blue == rgbValueCheck.blue);
#endif
    ColorHexagonSetColor(hWnd, hsvValue);
}

static void ColorHexagonHandleMouseLButtonUp(HWND hColorHexagonWnd, int xPos, int yPos, InternalColorHexagonState *pState)
{
    // Implementation for handling mouse down event
    pState->we_are_tracking_mouse = false;
}

static void ColorHexagonHandleMouseMove(HWND hColorHexagonWnd, int xPos, int yPos, InternalColorHexagonState *pState)
{
    // Implementation for handling mouse down event
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

                // Draw the color indicator based on the current HSV color
                DrawColorIndicator(hdc, pState);
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case CHM_SETCOLOR:
        {
            PUNITHSVTRIPLE pColor = (PUNITHSVTRIPLE)lParam;

            {
                _TCHAR buffer[128];

                _sntprintf_s(buffer,
                             _countof(buffer),
                             _TRUNCATE,
                             _TEXT("%s: CHM_SETCOLOR { Hue: %f, Saturation: %f, Value: %f }\n"),
                             ColorHexagonClassName,
                             pColor->hue,
                             pColor->saturation,
                             pColor->value);
                OutputDebugString(buffer);
            }
            {
                InternalColorHexagonState *pState = RETRIEVE_USER_DATA(hWnd);

                if ( pState )
                {
                    pState->color = *pColor;
                    InvalidateRect(hWnd, nullptr, TRUE); // Force a repaint with the new color
                }
            }
        }
        break;
    case CHM_GETCOLOR:
        {
            InternalColorHexagonState *pState = RETRIEVE_USER_DATA(hWnd);

            if ( pState )
            {
                *(PUNITHSVTRIPLE)lParam = pState->color;

                {
                    _TCHAR buffer[64];

                    _sntprintf_s(buffer,
                                 _countof(buffer),
                                 _TRUNCATE,
                                 _TEXT("%s: CHM_GETCOLOR { Hue: %f, Saturation: %f, Value: %f }\n"),
                                 ColorHexagonClassName,
                                 pState->color.hue,
                                 pState->color.saturation,
                                 pState->color.value);
                    OutputDebugString(buffer);
                }
            }
        }
        break;
    case WM_MOUSEMOVE:
        {
            InternalColorHexagonState *pState = RETRIEVE_USER_DATA(hWnd);
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            {
                _TCHAR buffer[64];

                _sntprintf_s(buffer,
                             _countof(buffer),
                             _TRUNCATE,
                             _TEXT("%s: WM_MOUSEMOVE { X: %d, Y: %d }\n"),
                             ColorHexagonClassName,
                             xPos,
                             yPos);
                OutputDebugString(buffer);
            }
            ColorHexagonHandleMouseMove(hWnd, xPos, yPos, pState);
        }
        break;
    case WM_LBUTTONDOWN:
        {
            InternalColorHexagonState *pState = RETRIEVE_USER_DATA(hWnd);
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            {
                _TCHAR buffer[64];

                _sntprintf_s(buffer,
                             _countof(buffer),
                             _TRUNCATE,
                             _TEXT("%s: WM_LBUTTONDOWN { X: %d, Y: %d }\n"),
                             ColorHexagonClassName,
                             xPos,
                             yPos);
                OutputDebugString(buffer);
            }
            ColorHexagonHandleMouseLButtonDown(hWnd, xPos, yPos, pState);
        }
        break;
    case WM_LBUTTONUP:
        {
            InternalColorHexagonState *pState = RETRIEVE_USER_DATA(hWnd);
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            {
                _TCHAR buffer[64];

                _sntprintf_s(buffer,
                             _countof(buffer),
                             _TRUNCATE,
                             _TEXT("%s: WM_LBUTTONUP { X: %d, Y: %d }\n"),
                             ColorHexagonClassName,
                             xPos,
                             yPos);
                OutputDebugString(buffer);
            }
            ColorHexagonHandleMouseLButtonUp(hWnd, xPos, yPos, pState);
        }
        break;
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

extern "C" {

ATOM ColorHexagonRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = ColorHexagonWndProc;
    wcex.hInstance = hInstance;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszClassName = ColorHexagonClassName;

    return RegisterClassEx(&wcex);
}

BOOL ColorHexagonUnregisterClass(HINSTANCE hInstance)
{
    return UnregisterClass(ColorHexagonClassName, hInstance);
}

void ColorHexagonSetColor(HWND hwnd, UNITHSVTRIPLE color)
{
    SendMessage(hwnd, CHM_SETCOLOR, 0, (LPARAM)&color);
}

UNITHSVTRIPLE ColorHexagonGetColor(HWND hwnd)
{
    UNITHSVTRIPLE color;

    SendMessage(hwnd, CHM_GETCOLOR, 0, (LPARAM)&color);
    return color;
}

}
