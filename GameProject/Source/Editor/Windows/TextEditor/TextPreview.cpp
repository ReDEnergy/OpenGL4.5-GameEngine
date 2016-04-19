#include "TextPreview.h"

#include <iostream>
#include <include/math.h>

#include <Editor/GUI.h>

#include <QtWidgets/QLayout>
#include <QtWidgets/QTextEdit>


static unsigned int windowNumber = 0;

TextPreviewWindow::TextPreviewWindow()
{
	setWindowTitle("Code Viewer");
	InitUI();
}

void TextPreviewWindow::InitUI()
{
	qtTextEdit = new QTextEdit(this);
	qtTextEdit->setBaseSize(QSize(300, 200));
	qtTextEdit->setReadOnly(true);

	QFont font;
	QFontMetrics metrics(font);
	qtTextEdit->setTabStopWidth(8 * metrics.width(' '));

	qtLayout->addWidget(qtTextEdit);

	GUI::Set(QT_INSTACE::TEXT_PREVIEW, (void*)this);
}

void TextPreviewWindow::RenderText(const char* text)
{
	qtTextEdit->setPlainText(text);
}

