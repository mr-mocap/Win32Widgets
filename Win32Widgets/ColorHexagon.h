#pragma once

#include <windows.h>

#define CHM_SETCOLOR (WM_USER + 1)
#define CHM_GETCOLOR (WM_USER + 2)

extern "C" {

extern const LPCWSTR ColorHexagonClassName;

ATOM ColorHexagonRegisterClass(HINSTANCE hInstance);

BOOL ColorHexagonUnregisterClass(HINSTANCE hInstance);

void      ColorHexagonSetColor(HWND hwnd, RGBTRIPLE color);
RGBTRIPLE ColorHexagonGetColor(HWND hwnd);

}
