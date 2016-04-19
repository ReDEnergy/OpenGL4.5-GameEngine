#include <pch.h>
#include "EditorMain.h"

#include <functional> 
#include <stdlib.h>

#include <Editor/include/Editor.h>

// QT objects
#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QStatusBar>
#include <QFileDialog>
#include <QApplication>

// GameWorld
#include <Game/Game.h>


EditorMain::EditorMain(QWidget *parent)
	: QMainWindow(parent)
{
	SetupUI(this);

	for (auto &window : dockWindows) {
		window.second->Init();
	}

	for (auto &window : appWindows) {
		window.second->Init();
	}

	show();
}

EditorMain::~EditorMain()
{
}

void EditorMain::Run(QApplication *app)
{
	this->app = app;

	// TODO - this prevents the Main thread to close
	// should find an alternative to update the FrontEnd using a 24/30 FPS QTimer 
	// right now the Engine is in sync with the FrontEnd, UI updates can be blocked

	Manager::GetEvent()->EmitSync(EventType::FRONT_END_INITIALIZED);

	Engine::Run();
}

void EditorMain::close()
{
	cout << "Exit" << endl;
	Engine::Exit();
}

void EditorMain::closeEvent(QCloseEvent * event)
{
	close();
}

void EditorMain::SetupUI(QMainWindow *MainWindow) {

	setWindowIcon(*QtConfig::GetIcon("colorwheel.png"));

	auto appName = QtConfig::GetAppName() + " " + QtConfig::GetAppVersion();
	MainWindow->setWindowTitle(appName.c_str());
	MainWindow->resize(1280, 720);
	MainWindow->setDockOptions(QMainWindow::AnimatedDocks |
								QMainWindow::AllowNestedDocks |
								QMainWindow::AllowTabbedDocks);

	MainWindow->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::TabPosition::North);

	// ************************************************************************
	// Load styling

	ReloadStyleSheets();

	// ************************************************************************
	// File Picker

	auto *picker = new FilePicker();
	picker->setNameFilter("*.xml");
	GUI::Set(QT_INSTACE::FILE_PICKER, (void*) picker);

	// ************************************************************************
	// Menu Bar

	QMenuBar *menuBar = new QMenuBar(MainWindow);
	menuBar->setObjectName(QStringLiteral("menuBar"));
	menuBar->setGeometry(QRect(0, 0, 1051, 21));
	MainWindow->setMenuBar(menuBar);

	// Menu Entry
	{
		QMenu *menuEngine = new QMenu(menuBar);
		menuEngine->setObjectName(QStringLiteral("menuEngine"));
		menuEngine->setTitle("File");

		menuBar->addAction(menuEngine->menuAction());

		{
			QAction *action = new QAction(MainWindow);
			action->setText("Exit");
			action->setIcon(*QtConfig::GetIcon("power.png"));
			menuEngine->addAction(action);
			QObject::connect(action, &QAction::triggered, this, &EditorMain::close);
		}
	}

	// Menu Entry
	{
		QMenu *menu = new QMenu(menuBar);
		menu->setTitle("Tools");
		menuBar->addAction(menu->menuAction());

		// Submenu buttons
		{
			QAction *action = new QAction(MainWindow);
			action->setText("Reload StyleSheets");
			action->setIcon(*QtConfig::GetIcon("cmyk.png"));
			menu->addAction(action);
			QObject::connect(action, &QAction::triggered, this, &EditorMain::ReloadStyleSheets);
		}
	}

	// Menu Entry
	{
		QMenu *menu = new QMenu(menuBar);
		menu->setTitle("Help");
		menuBar->addAction(menu->menuAction());

		// Submenu buttons
		QAction *action = new QAction(MainWindow);
		action->setText("About");
		action->setIcon(*QtConfig::GetIcon("chat.png"));
		menu->addAction(action);

		appWindows["About"] = new AboutWindow();

		QObject::connect(action, &QAction::triggered, this, [&]() {
			appWindows["About"]->Toggle();
		});
	}

	// ************************************************************************
	// Toolbar

	QToolBar *toolbar = new QToolBar();
	toolbar->setWindowTitle("Toolbar");

	// Attach toobar to the main window
	MainWindow->addToolBar(toolbar);

	// ************************************************************************
	// Status Bar

	QStatusBar *statusBar = new QStatusBar(MainWindow);
	statusBar->setObjectName(QStringLiteral("statusBar"));
	MainWindow->setStatusBar(statusBar);

	//QMetaObject::connectSlotsByName(MainWindow);

	// ************************************************************************
	// Central Area

	QWidget *centeralWidget = new QWidget();
	centeralWidget->setFixedWidth(0);
	MainWindow->setCentralWidget(centeralWidget);

	// ************************************************************************
	// Attach windows

	// WARNING GameWindow will trigger OpenGL Context Switch
	dockWindows["CameraProp"] = new CameraPropertyEditor();
	dockWindows["GameWindow"] = new GameWindow();
	dockWindows["GeneralWindow"] = new GeneralWindow();
	dockWindows["ObjectProperties"] = new ObjectPropertiesWindow();
	dockWindows["ProfilingWindow"] = new ProfilingWindow();
	

	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["ObjectProperties"], Qt::Orientation::Horizontal);
	MainWindow->tabifyDockWidget(dockWindows["ObjectProperties"], dockWindows["CameraProp"]);

	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["ProfilingWindow"], Qt::Orientation::Vertical);
	MainWindow->tabifyDockWidget(dockWindows["ProfilingWindow"], dockWindows["GeneralWindow"]);

	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["GameWindow"], Qt::Orientation::Horizontal);

	// Left Dock
	dockWindows["GameWindow"]->setFloating(true);
	dockWindows["GameWindow"]->setFloating(false);

	dockWindows["ObjectProperties"]->raise();
	dockWindows["ProfilingWindow"]->raise();
}

void EditorMain::Update()
{
	//app->processEvents();
	auto SceneWindow = GUI::Get<GameWindow>(QT_INSTACE::_3D_SCENE_WINDOW);
	if (SceneWindow) {
		SceneWindow->Update();
		SceneWindow->Render();
	}
	dockWindows["ObjectProperty"]->Update();
}

void EditorMain::ReloadStyleSheets()
{
	for (auto &window : dockWindows) {
		window.second->ReloadStyleSheet();
	}

	for (auto &window : appWindows) {
		window.second->ReloadStyleSheet();
	}
}