#include "gl.h"
#include <mutex>

#include <iostream>

using namespace std;

static string GLerrorDescription[] = {
	"GL_INVALID_ENUM",						// 0x0500
	"GL_INVALID_VALUE",						// 0x0501
	"GL_INVALID_OPERATION",					// 0x0502
	"GL_STACK_OVERFLOW",					// 0x0503
	"GL_STACK_UNDERFLOW",					// 0x0504
	"GL_OUT_OF_MEMORY",						// 0x0505
	"GL_INVALID_FRAMEBUFFER_OPERATION" 		// 0x0506
};

#ifdef OPENGL_ES

static string EGLErrorDescription[][2] = {
	{ "EGL_SUCCESS",				"Function succeeded."},																					// 0x3000
	{ "EGL_NOT_INITIALIZED",		"EGL is not or could not be initialized, for the specified display." },									// 0x3001
	{ "EGL_BAD_ACCESS",				"EGL cannot access a requested resource." },															// 0x3002
	{ "EGL_BAD_ALLOC",				"EGL failed to allocate resources for the requested operation." },										// 0x3003
	{ "EGL_BAD_ATTRIBUTE",			"An unrecognized attribute or attribute value was passed in an attribute list." },						// 0x3004
	{ "EGL_BAD_CONFIG",				"An EGLConfig argument does not name a valid EGLConfig." },												// 0x3005
	{ "EGL_BAD_CONTEXT",			"An EGLContext argument does not name a valid EGLContext." },											// 0x3006
	{ "EGL_BAD_CURRENT_SURFACE",	"The current surface of the calling thread is a window, pbuffer, or pixmap that is no longer valid." },	// 0x3007
	{ "EGL_BAD_DISPLAY",			"An EGLDisplay argument does not name a valid EGLDisplay." },											// 0x3008
	{ "EGL_BAD_MATCH",				"Arguments are inconsistent; for example, an otherwise valid \
									context requires buffers(e.g.depth or stencil) not allocated by	an otherwise valid surface." },			// 0x3009
	{ "EGL_BAD_NATIVE_PIXMAP",		"An EGLNativePixmapType argument does not refer to a valid native pixmap." },							// 0x300A
	{ "EGL_BAD_NATIVE_WINDOW",		"An EGLNativeWindowType argument does not refer to a valid native window." },							// 0x300B
	{ "EGL_BAD_PARAMETER",			"One or more argument values are invalid." },															// 0x300C
	{ "EGL_BAD_SURFACE",			"An EGLSurface argument does not name a valid surface(window, pbuffer, or pixmap) \
									configured for rendering." },																			// 0x300D
	{ "EGL_CONTEXT_LOST",			"" },																									// 0x300E
};

#endif

void PrintGLErrorDescription(unsigned int glErr)
{
	cout << "\n[OpenGL Error]\n";
	cout << "\t[" << glErr << "] : " << GLerrorDescription[glErr - GL_INVALID_ENUM].c_str() << "\n";
}

#ifdef OPENGL_ES
void PrintEGLErrorDescription(unsigned int glErr)
{
	cout << "\n[OpenGL ES Error]\n";
	cout << "\t[" << glErr << "] : " << EGLErrorDescription[glErr - EGL_SUCCESS][0].c_str() << "\t" << EGLErrorDescription[glErr - EGL_SUCCESS][1].c_str() << "\n";
}
#endif

int OpenGL::CheckError(const char * file, int line, bool log)
{
	#ifdef _DEBUG
		#ifdef OPENGL_ES
			GLint glErr = eglGetError();
			if (glErr != EGL_SUCCESS)
			{
				PrintEGLErrorDescription(glErr);
				cout << "\t[File] : " << file << "\t[Line] : " << line << endl;
				return glErr;
			}
			#else
			GLenum glErr = glGetError();
			if (glErr != GL_NO_ERROR && log)
			{
				PrintGLErrorDescription(glErr);
				cout << "\t[File] : " << file << "\t[Line] : " << line << endl;
				return glErr;
			}
		#endif
	#endif
	return 0;
}

static std::mutex mutexGPU;

void OpenGL::AcquireLock()
{
	mutexGPU.lock();
}

void OpenGL::ReleaseLock()
{
	mutexGPU.unlock();
}
