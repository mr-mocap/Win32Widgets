#pragma once

#include <windows.h>
#include "Color.h"

#define CHM_SETCOLOR (WM_USER + 1)
#define CHM_GETCOLOR (WM_USER + 2)

#ifdef __cplusplus
extern "C" {
#endif

extern const LPCWSTR ColorHexagonClassName;

ATOM ColorHexagonRegisterClass(HINSTANCE hInstance);

BOOL ColorHexagonUnregisterClass(HINSTANCE hInstance);

void          ColorHexagonSetColor(HWND hwnd, UNITHSVTRIPLE color);
UNITHSVTRIPLE ColorHexagonGetColor(HWND hwnd);

#ifdef __cplusplus
}
#endif
