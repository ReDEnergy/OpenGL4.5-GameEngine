#include "QtSortableInput.h"

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <algorithm>

#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QStringList>

#include <include/glm.h>
#include <include/math.h>

using namespace std;

QtSortableInput::QtSortableInput(const char * label, unsigned int precision)
{
	qtLayout->setDirection(QBoxLayout::LeftToRight);
	qtLayout->setSpacing(0);

	precision = MIN(precision, 4);

	format = new char[20];
	sprintf(format, "%%.%df", precision);

	{
		qLabel = new QLabel(label);
		qLabel->setMinimumWidth(80);
		qLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

		prop = new QLineEdit();
		qtLayout->addWidget(qLabel);
		qtLayout->addWidget(prop);

		QObject::connect(prop, &QLineEdit::editingFinished, this, &QtSortableInput::Sort);
	}
}

void QtSortableInput::SetLabelWidth(int width)
{
	qLabel->setMinimumWidth(width);
}

void QtSortableInput::Sort()
{
	auto value = prop->text();
	value.replace(QRegExp("[a-zA-Z ]+"), "");
	QStringList qList = value.split(',');

	values.clear();
	values.reserve(qList.size());
	for (auto x : qList) {
		values.push_back(x.toFloat());
	}
	if (values.size() == 0)
	{
		SetValue("0");
		return;
	}
	sort(values.begin(), values.end());

	char text[10];
	string outPut("");
	for (auto v : values) {
		if (outPut.size())
			outPut += ", ";
		sprintf(text, format, v);
		outPut += text;
	}

	prop->setText(QString(outPut.c_str()));

	if (updateFunc)
		updateFunc(values);
}

void QtSortableInput::OnUserEdit(function<void(const vector<float>&)> func)
{
	updateFunc = func;
}

void QtSortableInput::SetValue(const char * value)
{
	prop->setText(QString(value));
	Sort();
}

const vector<float>& QtSortableInput::GetValues() const
{
	return values;
}
