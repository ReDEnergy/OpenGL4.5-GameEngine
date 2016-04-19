#include "AboutWindow.h"

// QT
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QPlainTextEdit>

AboutWindow::AboutWindow()
{
	setWindowTitle("About");
	SetIcon("chat.png");
	LoadStyleSheet("about-window.css");
	InitUI();
}

void AboutWindow::InitUI()
{
	setFixedSize(300, 150);

	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);

}