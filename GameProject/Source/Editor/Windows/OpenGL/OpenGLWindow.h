#pragma once
#include <QtGui/QWindow>

class QOpenGLContext;
class QtGLFW;
class QWidget;
class WindowObject;

class OpenGLWindow : public QWindow
{
	public:
		OpenGLWindow();
		~OpenGLWindow();

		void Init(WindowObject* sharedContext = nullptr);
		void EndFrame();
		QOpenGLContext* GetContext();
		bool SetAsCurrentContext();

	public:
		virtual void CenterCursor();
		virtual void HideCursor();
		virtual void ShowCursor();
		virtual bool SharesContext() const final;
		virtual WindowObject* GetWindowObject() const final;

	private:
		void KeyEvent(QKeyEvent* e, bool state);
		void MouseEvent(QMouseEvent * e, bool state);

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
		QtGLFW *qtGLFWEvents;
		WindowObject* sharedContext;
		bool focused;
		bool cursorClip;
		bool skipClipFrame;
		QPointF prevMousePos;

	protected:
		QOpenGLContext* qtGLContext;
};