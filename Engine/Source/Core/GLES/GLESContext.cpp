#ifdef OPENGL_ES

#include "GLESContext.h"

#include <Core/InputSystem.h>
#include <Core/WindowManager.h>

#include <include/gl.h>
#include <Core/GLES/NativeWin.h>

#include <Manager/Manager.h>
#include <Manager/ConfigFile.h>

//void OnNativeWinResize(int width, int height)
//{
	//if (ctx->nativeWin != NULL)
	//{
	//	ctx->nWindowWidth = width;
	//	ctx->nWindowHeight = height;
	//	glViewport(0, 0, width, height);
	//}
//}

//void OnNativeWinMouseMove(int mousex, int mousey, bool lbutton)
//{
	//if (lbutton)
	//{
	//	int oldx = ctx->nMouseX;
	//	int oldy = ctx->nMouseY;
	//	GLfloat yaw = ctx->rs.yaw;
	//	GLfloat pitch = ctx->rs.pitch;
	//	yaw += (oldx - mousex) * 720.0f / ctx->nWindowWidth;
	//	pitch += (oldy - mousey) * 360.0f / ctx->nWindowHeight;
	//	yaw = (abs(yaw) <  360) ? yaw : yaw / 360;
	//	pitch = (pitch <  90) ? pitch : 89;
	//	pitch = (pitch > -90) ? pitch : -89;
	//	ctx->rs.yaw = yaw;
	//	ctx->rs.pitch = pitch;
	//}
	//ctx->nMouseX = mousex;
	//ctx->nMouseY = mousey;
//}

GLESContext::GLESContext()
{
}

GLESContext::~GLESContext()
{
}

void GLESContext::Init()
{
	EGLint num_config;
	EGLBoolean result;

	EGLint attributes[] =
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_NONE
	};

	EGLint context_attributes[] = 
	{
		EGL_CONTEXT_MAJOR_VERSION, 3,
		EGL_CONTEXT_MINOR_VERSION, 1,
		EGL_NONE
	};

	// create native window
	EGLNativeDisplayType nativeDisplay;
	if (!OpenNativeDisplay(&nativeDisplay))
	{
		printf("Could not get open native display\n");
		return;
	}

	/* get an EGL display connection */
	eglDisplay = eglGetDisplay(nativeDisplay);
	CheckOpenGLError();

	/* initialize the EGL display connection */
	EGLint EGLmajor, EGLminor;
	result = eglInitialize(eglDisplay, &EGLmajor, &EGLminor);
	CheckOpenGLError();

	/* get an appropriate EGL frame buffer configuration */
	result = eglChooseConfig(eglDisplay, attributes, &eglConfig, 1, &num_config);
	CheckOpenGLError();

	/* create an EGL rendering context */
	eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, context_attributes);
	CheckOpenGLError();
	if (eglContext) {
		printf("[EGL Context] Created\n");
	} else {
		printf("[EGL Context] Creation failed\n");
	}
	Setup();
}

int GLESContext::Setup()
{
	// Get the native visual id
	int nativeVid;
	if (!eglGetConfigAttrib(eglDisplay, eglConfig, EGL_NATIVE_VISUAL_ID, &nativeVid))
	{
		printf("Could not get native visual id\n");
		CloseNativeDisplay(nativeDisplay);
		return GL_FALSE;
	}

	auto resolution = Manager::GetConfig()->windowProperties.resolution;

	EGLNativeWindowType nativeWin;
	if (!CreateNativeWin(nativeDisplay, resolution.x, resolution.y, nativeVid, &nativeWin))
	{
		printf("Could not create window\n");
		CloseNativeDisplay(nativeDisplay);
		return GL_FALSE;
	}

	// Create a surface for the main window
	eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, nativeWin, NULL);
	if (eglSurface == EGL_NO_SURFACE)
	{
		printf("Could not create EGL surface\n");
		DestroyNativeWin(nativeDisplay, nativeWin);
		CloseNativeDisplay(nativeDisplay);
		return GL_FALSE;
	}

	// Make the context and surface current
	EGLBoolean bsuccess = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
	if (!bsuccess)
	{
		printf("Could not activate EGL context\n");
		DestroyNativeWin(nativeDisplay, nativeWin);
		CloseNativeDisplay(nativeDisplay);
		return GL_FALSE;
	}

	// make sure an initial resize event is provided to the application
	//OnNativeWinResize(640, 480);

	return GL_TRUE;
}

EGLConfig GLESContext::GetContext() const
{
	return eglConfig;
}

EGLBoolean GLESContext::MakeCurrent() const
{
	return eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
}

void GLESContext::SwapBuffer() const
{
	eglSwapBuffers(eglDisplay, eglSurface);
}
#endif