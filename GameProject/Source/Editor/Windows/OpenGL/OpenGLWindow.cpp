#include "OpenGLWindow.h"

#include <Editor/OpenGL/QtGLFW.h>
#include <Editor/Config/QtConfig.h>

#include <QKeyEvent>
#include <QWidget>

#include <QOpenGLContext>
#include <QSurfaceFormat>

#ifdef _WIN32
	#include <QtPlatformHeaders/QWGLNativeContext>
#elif __linux__
//	#include <QtPlatformHeaders/QGLXNativeContext>
#endif

#define GL_DISABLE_GLEW
#include <include/gl_native.h>

#include <Engine.h>

OpenGLWindow::OpenGLWindow()
{
	sharedContext = nullptr;
	setSurfaceType(QSurface::OpenGLSurface);
	container = QWidget::createWindowContainer(this);

	qtGLFWEvents = new QtGLFW();
}

OpenGLWindow::~OpenGLWindow()
{
}

void OpenGLWindow::Init(WindowObject *sharedContext)
{
	// Create surface context
	qtGLContext = new QOpenGLContext();

	if (sharedContext)
	{
		sharedContext->UseNativeHandles(true);
		this->sharedContext = sharedContext;

		auto res = sharedContext->props.resolution;
		container->setMinimumWidth(res.x);
		container->setMinimumHeight(res.y);
		QtConfig::SetWidgetStyleSheet(container, "OpenGL.css");

		// Create a new OpenGL context using a native OS handle
		QOpenGLContext* qtctx = new QOpenGLContext();

		#ifdef _WIN32
			auto glfwCTX = glfwGetWGLContext(sharedContext->GetGLFWWindow());
			auto window = glfwGetWin32Window(sharedContext->GetGLFWWindow());
			auto newNative = new QWGLNativeContext(glfwCTX, window);
			qtctx->setNativeHandle(QVariant::fromValue<QWGLNativeContext>(*newNative));
		#endif

		qtctx->create();
		qtctx->makeCurrent(this);

		// Shared the context with the rendering context
		qtGLContext->setShareContext(qtctx);
	}

	// Actually create the context
	qtGLContext->create();

	// Set OpenGL format
	QSurfaceFormat format;
	format.setVersion(3, 3);
	format.setSwapInterval(0);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setSwapBehavior(QSurfaceFormat::SwapBehavior::SingleBuffer);
	setFormat(format);

	// Test sharing
	auto ctx = qtGLContext->shareContext();
	//auto nativeHandle = qtGLContext->nativeHandle();
	//auto windowsNative = nativeHandle.value<QWGLNativeContext>();
}

void OpenGLWindow::EndFrame()
{
	qtGLContext->swapBuffers(this);
}

QOpenGLContext * OpenGLWindow::GetContext()
{
	return qtGLContext;
}

bool OpenGLWindow::SetAsCurrentContext()
{
	return qtGLContext->makeCurrent(this);
}

void OpenGLWindow::CenterCursor()
{
	skipClipFrame = true;
	cursor().setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
	sharedContext->CenterPointer();
}

void OpenGLWindow::HideCursor()
{
	skipClipFrame = true;
	cursorClip = true;
	QCursor c = cursor();
	c.setShape(Qt::BlankCursor);
	setCursor(c);
}

void OpenGLWindow::ShowCursor()
{
	cursorClip = false;
	QCursor c = cursor();
	c.setShape(Qt::ArrowCursor);
	setCursor(c);
}

bool OpenGLWindow::SharesContext() const
{
	return (sharedContext != nullptr);
}

WindowObject * OpenGLWindow::GetWindowObject() const
{
	return sharedContext;
}

void OpenGLWindow::KeyEvent(QKeyEvent * e, bool state)
{
	if (!focused) return;

	int mods = qtGLFWEvents->GetModifiers();
	int key = qtGLFWEvents->GetKeyButton(e->key());
	InputSystem::KeyCallback(nullptr, key, GLFW_KEY_UNKNOWN, state, mods);

	//cout << "[QT] " << e->key() << "\t[GLFW] " << key << "\t[MODS] " << mods << endl;
}

void OpenGLWindow::MouseEvent(QMouseEvent * e, bool state)
{
	if (!focused) return;
	int mods = qtGLFWEvents->GetModifiers();
	int glfwButton = qtGLFWEvents->GetMouseButton(e->button());

	prevMousePos = e->windowPos();
	InputSystem::MouseClick(nullptr, glfwButton, state, mods);

	//cout << qtButton << "[" << e->windowPos().x() << ", " << e->windowPos().y() << "]" << endl;
}

void OpenGLWindow::keyPressEvent(QKeyEvent * e)
{
	KeyEvent(e, GLFW_PRESS);
}

void OpenGLWindow::keyReleaseEvent(QKeyEvent * e)
{
	KeyEvent(e, GLFW_RELEASE);
}

unsigned int countMovements = 0;

void OpenGLWindow::mouseMoveEvent(QMouseEvent * e)
{
	if (!focused) return;

	if (cursorClip && skipClipFrame)
	{
		skipClipFrame = false;
		return;
	}

	prevMousePos = e->windowPos();
	InputSystem::CursorMove(sharedContext->GetGLFWWindow(), e->windowPos().x(), e->windowPos().y());

	if (cursorClip) {
		CenterCursor();
	}
}

void OpenGLWindow::mousePressEvent(QMouseEvent * e)
{
	if (!focused) return;

	if (e->button() == Qt::MouseButton::RightButton) {
		HideCursor();
		CenterCursor();
	}

	MouseEvent(e, GLFW_PRESS);
}

void OpenGLWindow::mouseReleaseEvent(QMouseEvent * e)
{
	if (!focused) return;

	if (e->button() == Qt::MouseButton::RightButton) {
		ShowCursor();
	}

	MouseEvent(e, GLFW_RELEASE);
}

void OpenGLWindow::wheelEvent(QWheelEvent * e)
{
	if (!focused) return;
}

void OpenGLWindow::focusInEvent(QFocusEvent * e)
{
	focused = true;
	//cout << "[Window] Focus In" << endl;
}

void OpenGLWindow::focusOutEvent(QFocusEvent * e)
{
	focused = false;
	//cout << "[Window] Focus Out" << endl;
	ShowCursor();
}

void OpenGLWindow::resizeEvent(QResizeEvent * e)
{
	//cout << "[Window] [Resize] " << e->size().width() << ", "<< e->size().height() << endl;
}
