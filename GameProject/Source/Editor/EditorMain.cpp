#include <pch.h>
#include "EditorMain.h"

#include <functional> 
#include <stdlib.h>

#include <GUI/GUI.h>
#include <Editor/include/Editor.h>

// QT objects
#include <QMenuBar>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QStatusBar>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>

// GameWorld
#include <Game/Game.h>

#include <Modules/RegisteredModuleUI.h>

using namespace std;

EditorMain::EditorMain(QApplication &app)
	: QMainWindow(nullptr), app(app)
{
	cout << "[Qt Version]: " << QT_VERSION_MAJOR << "." << QT_VERSION_MINOR << "." << QT_VERSION_PATCH << endl;

	GUI::GUIControl::Init();

	SetupUI(this);

	for (auto &window : dockWindows) {
		window.second->Init();
	}

	for (auto &window : appWindows) {
		window.second->Init();
	}

	updateUI = false;
}

EditorMain::~EditorMain()
{
}

void EditorMain::Run()
{
	int appWidth = 1400;
	int appHeight = 800;
	resize(appWidth, appHeight);

	Manager::GetEvent()->EmitSync(EventType::FRONT_END_INITIALIZED);

	show();
	CenterWindow();

	uiUpdateTimer = new QTimer();
	uiUpdateTimer->setInterval(16);
	connect(uiUpdateTimer, &QTimer::timeout, this, [this]() {
		Update();
	});
	uiUpdateTimer->start();

	app.exec();
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

	// ************************************************************************
	// Menu Bar

	QMenuBar *menuBar = new QMenuBar(MainWindow);
	menuBar->setObjectName(QStringLiteral("menuBar"));
	//menuBar->setGeometry(QRect(0, 0, 1051, 21));
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

		// Sub-menu buttons
		{
			QAction *action = new QAction(MainWindow);
			action->setText("Reload StyleSheets");
			action->setIcon(*QtConfig::GetIcon("cmyk.png"));
			menu->addAction(action);
			QObject::connect(action, &QAction::triggered, this, &EditorMain::ReloadStyleSheets);
		}

		{
			QAction *action = new QAction(MainWindow);
			action->setText("Center to screen");
			action->setIcon(*QtConfig::GetIcon("focus.png"));
			menu->addAction(action);
			QObject::connect(action, &QAction::triggered, this, &EditorMain::CenterWindow);
		}

		{
			QAction *action = new QAction(MainWindow);
			action->setText("Toggle Scrolling");
			action->setIcon(*QtConfig::GetIcon("focus.png"));
			menu->addAction(action);
			QObject::connect(action, &QAction::triggered, this, [this]() {
				auto value = dockWindows["ProfilingWindow"]->IsScrollable();
				dockWindows["ProfilingWindow"]->SetScrollable(!value);
			});
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

	// Game Window
	{
		QToolButton *button = new QToolButton();
		button->setText("GameWindow");
		button->setMaximumWidth(100);
		button->setIcon(*QtConfig::GetIcon("gamecontroller.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		QObject::connect(button, &QToolButton::clicked, this, []() {
			Manager::GetEvent()->EmitAsync("Game", "ToggleGameWindow", nullptr);
		});
		toolbar->addWidget(button);
	}

	// Render to Oculus Rift
	{
		QToolButton *button = new QToolButton();
		button->setText("OculusVR");
		button->setMaximumWidth(80);
		button->setIcon(*QtConfig::GetIcon("lens.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		QObject::connect(button, &QToolButton::clicked, this, []() {
			Manager::GetEvent()->EmitAsync("Game", "OculusRift-ToggleRendering", nullptr);
		});
		toolbar->addWidget(button);
	}

	// Toggle V-Sync
	{
		QToolButton *button = new QToolButton();
		button->setText("V-Sync");
		button->setMaximumWidth(80);
		button->setIcon(*QtConfig::GetIcon("frames.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		QObject::connect(button, &QToolButton::clicked, this, []() {
			Manager::GetEvent()->EmitAsync("Game", "V-Sync-Toggle", nullptr);
		});
		toolbar->addWidget(button);
	}

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
	//MainWindow->centralWidget()->hide();

	// ************************************************************************
	// Attach windows

	SetupModuleUIs();

	// WARNING GameWindow will trigger OpenGL Context Switch
	// Create this also through the *DockWindow* template
	dockWindows["GameWindow"] = new GameWindow();

	// Left Dock Area
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["SceneHierarchy"], Qt::Orientation::Horizontal);

	// Center Area
	MainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dockWindows["GameWindow"], Qt::Orientation::Horizontal);

	// Right Dock Area
	MainWindow->splitDockWidget(dockWindows["GameWindow"], dockWindows["ObjectProperties"], Qt::Orientation::Horizontal);
	MainWindow->splitDockWidget(dockWindows["ObjectProperties"], dockWindows["ProfilingWindow"], Qt::Orientation::Vertical);

	MainWindow->tabifyDockWidget(dockWindows["ObjectProperties"], dockWindows["GeneralWindow"]);
#ifdef KINECT_SENSOR
	MainWindow->tabifyDockWidget(dockWindows["ObjectProperties"], dockWindows["KinectSensor"]);
#endif
	MainWindow->tabifyDockWidget(dockWindows["ObjectProperties"], dockWindows["LeapSensor"]);
	MainWindow->tabifyDockWidget(dockWindows["ObjectProperties"], dockWindows["MotionSystem"]);
	MainWindow->tabifyDockWidget(dockWindows["ObjectProperties"], dockWindows["LeapControl"]);

	MainWindow->tabifyDockWidget(dockWindows["ProfilingWindow"], dockWindows["CameraProperties"]);

	dockWindows["GameWindow"]->setFloating(true);
	dockWindows["GameWindow"]->setFloating(false);

	dockWindows["SceneHierarchy"]->raise();
	dockWindows["ObjectProperties"]->raise();
}

void EditorMain::SetupModuleUIs()
{
	for (auto &module : registeredUIModules)
	{
		auto M = static_cast<DockWindow*>(module->Allocate());
		M->InitUI();

		auto name = M->windowTitle().toStdString();
		dockWindows[name] = M;
		moduleWindows.push_back(M);

		// UI zones are not defined at this point
		// tabifyDockWidget(dockWindows["ObjectProperties"], M);
	}
}

void EditorMain::CenterWindow()
{
	// Center the application on the main screen at startup
	auto desktopInfo = app.desktop()->screenGeometry();
	int posX = desktopInfo.width() - this->width();
	int posY = desktopInfo.height() - this->height();
	this->setGeometry(posX / 2, posY / 2, this->width(), this->height());
}

void EditorMain::Update()
{
	//app.processEvents();

	dockWindows["GameWindow"]->Update();

	// Update UI only at half frequency
	if (updateUI) {
		for (auto &window : moduleWindows) {
			window->Update();
		}
	}
	updateUI = !updateUI;
}

void EditorMain::ReloadStyleSheets()
{
	QtConfig::SetWidgetStyleSheet(this, "app.css");

	for (auto &window : dockWindows) {
		window.second->ReloadStyleSheet();
	}

	for (auto &window : appWindows) {
		window.second->ReloadStyleSheet();
	}

	for (auto &window : moduleWindows) {
		window->ReloadStyleSheet();
	}
}