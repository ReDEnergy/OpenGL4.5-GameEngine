#pragma once
#include <QtGui/QWindow>

class QOpenGLContext;
class QtGLFW;
class QWidget;
class WindowObject;

struct Viewport
{
	Viewport()
	{
		x = y = 0;
		width = height = 1;
	}
	Viewport(int x, int y, int width, int height)
		: x(x), y(y), width(width), height(height) {}

	int x;
	int y;
	int width;
	int height;
};

class OpenGLWindow : public QWindow
{
	public:
		OpenGLWindow();
		~OpenGLWindow();

		void Init(WindowObject* sharedContext = nullptr);
		void EndFrame();
		QOpenGLContext* GetContext();
		bool SetAsCurrentContext();
		void SetViewport(int x, int y, int width, int height);

	public:
		virtual void CenterCursor();
		virtual void HideCursor();
		virtual void ShowCursor();
		virtual bool SharesContext() const final;
		virtual WindowObject* GetWindowObject() const final;

	private:
		void KeyEvent(QKeyEvent* e, bool state);
		void MouseEvent(QMouseEvent * e, bool state);
		void SubmitMouseClick(QMouseEvent * e);

	protected:
		virtual void keyPressEvent(QKeyEvent* e);
		virtual void keyReleaseEvent(QKeyEvent* e);
		virtual void mouseMoveEvent(QMouseEvent* e);
		virtual void mousePressEvent(QMouseEvent* e);
		virtual void mouseReleaseEvent(QMouseEvent* e);
		virtual void wheelEvent(QWheelEvent* e);
		virtual void focusInEvent(QFocusEvent* e);
		virtual void focusOutEvent(QFocusEvent* e);
		virtual void resizeEvent(QResizeEvent * e);

	public:
		QWidget *container;

	private:
		Viewport viewport;
		QtGLFW *qtGLFWEvents;
		WindowObject* sharedContext;
		bool focused;
		bool cursorClip;
		bool skipClipFrame;
		QPointF prevMousePos;

	protected:
		QOpenGLContext* qtGLContext;
};