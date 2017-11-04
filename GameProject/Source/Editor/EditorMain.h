#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <Editor/include/QtForward.h>
#include <QMainWindow>
#include <QApplication>

class DockWindow;
class CustomWidget;
class GameWindow;

class EditorMain : public QMainWindow
{
	public:
		EditorMain(QApplication &app);
		~EditorMain();

		void Run();
		void Update();

	private:
		void SetupUI(QMainWindow * MainWindow);
		void SetupModuleUIs();
		void CenterWindow();

		// Toolbar actions
		void ReloadStyleSheets();

		// Qt Events
		void close();
		void closeEvent(QCloseEvent *event);

	private:
		bool updateUI;
		QTime *qtTime;
		QTimer *uiUpdateTimer;
		QApplication &app;
		QComboBox *dropdown;

		std::unordered_map<std::string, DockWindow*> dockWindows;
		std::unordered_map<std::string, CustomWidget*> appWindows;
		std::vector<DockWindow*> moduleWindows;

};

class GameEngine : public QApplication
{
	public:
		GameEngine(int argc, char *argv[])
			: QApplication(argc, argv)
		{
			window = new EditorMain(*this);
		}

		void Start() {
			window->Run();
		}

	private:
		EditorMain *window;
};