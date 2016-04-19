#include "QtInput.h"

#include <QStyledItemDelegate>
#include <QComboBox>

// ***************************************************************************
// Simple Float Input component

SimpleFloatInput::SimpleFloatInput(const char * label, const char * unit, unsigned int precision, bool readOnly)
{
	acceptNegativeValues = true;
	setProperty("SimpleFloatInput", "");

	qtLayout->setDirection(QBoxLayout::LeftToRight);
	qtLayout->setSpacing(0);

	SetPrecision(precision);
	SetUnit(unit);
	{
		qLabel = new QLabel(label);
		qLabel->setObjectName("SimpleFloatInputLabel");
		qLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

		prop = new QLineEdit();
		prop->setReadOnly(readOnly);
		qtLayout->addWidget(qLabel);
		qtLayout->addWidget(prop);

		QObject::connect(prop, &QLineEdit::editingFinished, this, [&]() {
			auto value = prop->text();
			value.replace(QRegExp("[a-zA-Z \\/]+"), "");
			SetValue(value.toFloat());
		});

		this->value = 0.1f;
		SetValue(0);
	}
}

SimpleFloatInput::~SimpleFloatInput()
{
	delete inputBuffer;
	delete format;
}

void SimpleFloatInput::SetReadOnly(bool value)
{
	prop->setReadOnly(value);
}

void SimpleFloatInput::OnUserEdit(function<void(float)> func)
{
	updateFunc = func;
}

void SimpleFloatInput::SetValue(float value)
{
	float newValue = int(value * toPrecision) / (float)toPrecision;
	if (this->value == newValue)
		return;

	this->value = acceptNegativeValues ? newValue : abs(newValue);
	sprintf(inputBuffer, format, this->value);
	prop->setText(inputBuffer);

	if (updateFunc)
		updateFunc(this->value);
}

void SimpleFloatInput::SetPrecision(float precision)
{
	this->precision = min(precision, 6.0f);
	toPrecision = pow(10, this->precision);
}

void SimpleFloatInput::SetUnit(const char * unit)
{
	auto inputLength = strlen(unit) + 10;
	format = new char[inputLength];
	inputBuffer = new char[inputLength];
	sprintf(format, "%%.%df %s", precision, unit);
}

void SimpleFloatInput::AcceptNegativeValues(bool value)
{
	acceptNegativeValues = value;
}

float SimpleFloatInput::GetValue() {
	return value;
}

// ***************************************************************************
// Simple CheckBox component

SimpleCheckBox::SimpleCheckBox(const char * label, bool checked)
{
	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
	setProperty("SimpleCheckBox", "");
	qtLayout->setDirection(QBoxLayout::LeftToRight);
	qtLayout->setSpacing(0);

	{
		qLabel = new QLabel(label);
		qLabel->setMinimumWidth(100);
		qLabel->setObjectName("SimpleCheckBoxLabel");
		qLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

		prop = new QCheckBox();
		qtLayout->addWidget(qLabel);
		qtLayout->addWidget(prop);

		QObject::connect(prop, &QCheckBox::clicked, this, [&](bool checked) {
			SetValue(checked);
		});

		SetValue(checked);
	}
}

void SimpleCheckBox::SetLabelWidth(int width)
{
	qLabel->setFixedWidth(width);
}

void SimpleCheckBox::SetValue(bool checked)
{
	value = checked;
	prop->setCheckState(checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
	if (updateFunc)
		updateFunc(value);
}

void SimpleCheckBox::OnUserEdit(function<void(bool)> func)
{
	updateFunc = func;
}

bool SimpleCheckBox::GetValue()
{
	return value;
}

// ***************************************************************************
// Simple DropDown component

SimpleDropDown::SimpleDropDown(const char * label)
{
	update = true;
	setProperty("SimpleDropDown", "");

	qtLayout->setDirection(QBoxLayout::LeftToRight);
	qtLayout->setSpacing(0);

	qLabel = new QLabel(label);
	qLabel->setMinimumWidth(100);
	qLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	qtLayout->addWidget(qLabel);

	dropdown = new QComboBox();

	QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
	dropdown->setItemDelegate(itemDelegate);
	qtLayout->addWidget(dropdown);

	void (QComboBox::* indexChangedSignal)(int index) = &QComboBox::currentIndexChanged;
	QObject::connect(dropdown, indexChangedSignal, this, [&](int index) {
		if (updateFunc && update)
			updateFunc(dropdown->itemData(index));
	});
}

void SimpleDropDown::AddOption(const char *name, QVariant value)
{
	dropdown->addItem(name, value);
}

void SimpleDropDown::RemoveOption(uint index)
{
	dropdown->removeItem(index);
}

void SimpleDropDown::RemoveOptions()
{
	update = false;
	dropdown->clear();
}

void SimpleDropDown::ResumeCallbacks()
{
	update = true;
}

void SimpleDropDown::StopCallbacks()
{
	update = false;
}

void SimpleDropDown::SetLabelWidth(int width)
{
	qLabel->setFixedWidth(width);
}

void SimpleDropDown::SetIndex(unsigned int index)
{
	dropdown->setCurrentIndex(index);
}

void SimpleDropDown::SetValue(QVariant value)
{
	auto typeIndex = dropdown->findData(value);
	if (typeIndex >= 0)
		dropdown->setCurrentIndex(typeIndex);
}

void SimpleDropDown::OnChange(function<void(QVariant)> func)
{
	updateFunc = func;
}

bool SimpleDropDown::GetValue()
{
	return false;
}