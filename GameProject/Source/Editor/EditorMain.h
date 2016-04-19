#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

#include <Editor/include/QTForward.h>
#include <QMainWindow>
#include <QApplication>

using namespace std;

class DockWindow;
class CustomWidget;
class GameWindow;

class EditorMain : public QMainWindow
{
	public:
		EditorMain(QWidget *parent = 0);
		~EditorMain();

		void Run(QApplication * app);
		void Update();

	private:
		void close();
		void closeEvent(QCloseEvent *event);
		void Config();

		void SetupUI(QMainWindow * MainWindow);
		void TestEvent();

		// Toolbar actions
		void ReloadStyleSheets();

	private:
		bool shouldQuit = false;
		QTime *qtTime;
		QTimer *qtTimer;
		QApplication *app;
		QComboBox *dropdown;

		unordered_map<string, DockWindow*> dockWindows;
		unordered_map<string, CustomWidget*> appWindows;
};

class GameEngine : public QApplication
{
	public:
		GameEngine(int argc, char *argv[])
			: QApplication(argc, argv)
		{
			auto window = new EditorMain();
			window->Run(this);
		}

	private:
		void lastWindowClosed() {
			cout << "LAST WINDOW CLOSED" << endl;
		}
};