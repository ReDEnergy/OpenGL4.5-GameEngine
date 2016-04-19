#pragma once
#ifdef OPENGL_ES

#include <include/dll_export.h>
#include <include/glm.h>

/*
 * Class WindowObject
 */

class DLLExport GLESContext
{
	friend class WindowManager;

	public:
		GLESContext();
		~GLESContext();

		void Init();
		EGLConfig GetContext() const;
		EGLBoolean MakeCurrent() const;
		void SwapBuffer() const;

	private:
		int Setup();

	private:
		bool success;
		EGLNativeDisplayType nativeDisplay;
		EGLDisplay eglDisplay;
		EGLSurface eglSurface;
		EGLConfig eglConfig;
		EGLContext eglContext;
};
#endif