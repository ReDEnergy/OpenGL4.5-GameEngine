#include "SimpleTextBox.h"

#include <QLabel>
#include <QlineEdit>

SimpleTextBox::SimpleTextBox(const char * label, const char * value, bool readOnly)
{
	qtLayout->setDirection(QBoxLayout::LeftToRight);
	qtLayout->setSpacing(0);
	setProperty("SimpleTextBox", 0);

	// Label
	qLabel = new QLabel(label);
	qLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	// Input Box
	prop = new QLineEdit();
	prop->setReadOnly(readOnly);
	if (value)
		prop->setText(value);

	// Attach to elements to root element
	qtLayout->addWidget(qLabel);
	qtLayout->addWidget(prop);

	QObject::connect(prop, &QLineEdit::editingFinished, this, [&]() {
		auto value = prop->text();
		if (updateFunc)
			updateFunc(value.toStdString().c_str());
	});
}

void SimpleTextBox::SetReadOnly(bool value)
{
	prop->setReadOnly(value);
}

void SimpleTextBox::SetValue(const char* value)
{
	prop->setText(value);
}

void SimpleTextBox::OnUserEdit(function<void(const char*)> func)
{
	updateFunc = func;
}

const char * SimpleTextBox::GetValue() const
{
	return prop->text().toStdString().c_str();
}