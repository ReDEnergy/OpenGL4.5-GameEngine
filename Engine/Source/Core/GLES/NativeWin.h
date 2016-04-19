#pragma once

#ifdef _WIN32

#include <EGL/egl.h>

bool OpenNativeDisplay(EGLNativeDisplayType* nativedisp_out);

void CloseNativeDisplay(EGLNativeDisplayType nativedisp);

bool CreateNativeWin(EGLNativeDisplayType nativedisp, int width, int height, int visid, EGLNativeWindowType* nativewin_out);

void DestroyNativeWin(EGLNativeDisplayType nativedisp, EGLNativeWindowType nativewin);

bool UpdateNativeWin(EGLNativeDisplayType nativedisp, EGLNativeWindowType nativewin);

#endif