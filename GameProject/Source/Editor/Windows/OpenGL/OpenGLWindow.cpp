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

using namespace std;

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
	CheckOpenGLError();

	//setMinimumHeight(480);

	if (sharedContext)
	{
		sharedContext->UseNativeHandles(true);
		this->sharedContext = sharedContext;

		setMinimumHeight(sharedContext->props.aspectRatio);

		container->setMinimumWidth(640);
		container->setMinimumHeight(480);
		container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		setHeight(720);
		int heightPX = height();
		setWidth(heightPX * sharedContext->props.aspectRatio);

		QtConfig::SetWidgetStyleSheet(container, "OpenGL.css");

		// Create a new OpenGL context using a native OS handle
		QOpenGLContext* qtctx = new QOpenGLContext();
		CheckOpenGLError();

		#ifdef _WIN32
			auto glfwCTX = glfwGetWGLContext(sharedContext->GetGLFWWindow());
			auto window = glfwGetWin32Window(sharedContext->GetGLFWWindow());
			auto newNative = new QWGLNativeContext(glfwCTX, window);
			qtctx->setNativeHandle(QVariant::fromValue<QWGLNativeContext>(*newNative));
		#endif

		auto status = qtctx->create();
		status = qtctx->makeCurrent(this);

		// Shared the context with the rendering context
		qtGLContext->setShareContext(qtctx);
	}

	// Actually create the context
	auto status = qtGLContext->create();
	CheckOpenGLError();

	// Set OpenGL format
	QSurfaceFormat format;
	format.setVersion(3, 3);
	format.setSwapInterval(0);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setSwapBehavior(QSurfaceFormat::SwapBehavior::SingleBuffer);
	setFormat(format);
	CheckOpenGLError();

	// Test sharing
	auto ctx = qtGLContext->shareContext();
	auto nativeHandle = qtGLContext->nativeHandle();
	auto windowsNative = nativeHandle.value<QWGLNativeContext>();
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

void OpenGLWindow::SetViewport(int x, int y, int width, int height)
{
	viewport = Viewport(x, y, width, height);
	glViewport(x, y, width, height);
}

void OpenGLWindow::CenterCursor()
{
	skipClipFrame = true;
	cursor().setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
	prevMousePos = QPoint(width() / 2, height() / 2);
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
	InputSystem::KeyCallback(sharedContext->window, key, GLFW_KEY_UNKNOWN, state, mods);

	//cout << "[QT] " << e->key() << "\t[GLFW] " << key << "\t[MODS] " << mods << endl;
}

void OpenGLWindow::MouseEvent(QMouseEvent * e, bool state)
{
	if (!focused) return;
	int mods = qtGLFWEvents->GetModifiers();
	int glfwButton = qtGLFWEvents->GetMouseButton(e->button());

	prevMousePos = e->windowPos();
	InputSystem::MouseClick(sharedContext->window, glfwButton, state, mods);

//	cout << "[" << e->windowPos().x() << ", " << e->windowPos().y() << "]" << endl;
}

void OpenGLWindow::SubmitMouseClick(QMouseEvent * e)
{
	auto currentPos = e->windowPos();
	auto posX = ((currentPos.x() - viewport.x) * sharedContext->props.resolution.x / viewport.width);
	auto posY = ((currentPos.y() - viewport.y) * sharedContext->props.resolution.y / viewport.height);
	InputSystem::CursorMove(sharedContext->GetGLFWWindow(), posX, posY);
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

	//cout << prevMousePos.x() << " " << prevMousePos.y() << endl;
	//cout << currentPos.x() << " " << currentPos.y() << "\t\t" << deltaPos.x() << " " << deltaPos.y() << endl;

	if (cursorClip && skipClipFrame)
	{
		prevMousePos = QPoint(width() / 2, height() / 2);
		skipClipFrame = false;
		return;
	}

	if (cursorClip) {
		auto currentPos = e->windowPos();
		auto deltaPos = currentPos - prevMousePos;
		InputSystem::CursorMove(sharedContext->GetGLFWWindow(), sharedContext->props.cursorPos.x + deltaPos.x(), sharedContext->props.cursorPos.y + deltaPos.y());
	}
	else {
		SubmitMouseClick(e);
	}

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

	if (e->button() == Qt::MouseButton::LeftButton) {
		SubmitMouseClick(e);
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
