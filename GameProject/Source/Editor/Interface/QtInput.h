#pragma once

#include <vector>
#include <string>
#include <functional>
#include <iostream>

#include <Editor/Utils/Utils.h>
#include <Editor/Windows/Interface/CustomWidget.h>

#include <QLabel>
#include <QLineEdit>
#include <QEvent>
#include <QCheckBox>
#include <QRegExp>
#include <QRegExpValidator>

#include <include/glm.h>
#include <include/math.h>

using namespace std;

// ***************************************************************************
// GLM UI component

template <class T>
class GLMVecComponent
	: public CustomWidget
{
	public:
		GLMVecComponent(const char* label, T vec)
		{
			value = -vec;
			value.x += 1;
			updateFunc = nullptr;

			qtLayout->setDirection(QBoxLayout::LeftToRight);
			qtLayout->setSpacing(0);

			{
				auto l = new QLabel(label);
				l->setMinimumWidth(65);
				l->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
				qtLayout->addWidget(l);
			}

			char *format[4] = { "x", "y", "z", "w" };

			for (unsigned int i = 0; i < vec.length(); i++)
			{
				auto l = new QLabel(format[i]);
				l->setMinimumWidth(15);
				l->setAlignment(Qt::AlignCenter);
				prop[i] = new QLineEdit();
				prop[i]->setMinimumWidth(40);
				prop[i]->setValidator(QtTextValidator::GetFloatValidator());
				qtLayout->addWidget(l);
				qtLayout->addWidget(prop[i]);

				auto lineEdit = prop[i];
				QObject::connect(prop[i], &QLineEdit::editingFinished, this, [&, i](){
					value[i] = prop[i]->text().toFloat();
					if (updateFunc)
						updateFunc(value);
				});
			}
		}

		~GLMVecComponent() {};

		void OnUserEdit(function<void(T)> func) {
			updateFunc = func;
		}

		void SetValue(const T &vec)
		{
			if (value == vec)
				return;
			value = vec;
			char buff[20];
			for (unsigned int i = 0; i < vec.length(); i++)
			{
				sprintf(buff, "%.3f", vec[i]);
				prop[i]->setText(buff);
			}
		}

	private:
		T value;
		function<void(T)> updateFunc;
		QLineEdit *prop[4];
};

class SimpleFloatInput
	: public CustomWidget
{
	private:
		SimpleFloatInput();

	public:
		
		SimpleFloatInput(const char* label, const char* unit = "", unsigned int precision = 2, bool readOnly = false);
		~SimpleFloatInput();

		void SetReadOnly(bool value);
		void SetValue(float value);
		void SetPrecision(float precision);
		void SetUnit(const char* unit);

		void OnUserEdit(function<void(float)> func);

		void AcceptNegativeValues(bool value);
		float GetValue();

	private:
		bool acceptNegativeValues;
		float value;
		int precision;
		int toPrecision;
		char *format;
		char *inputBuffer;
		QLabel *qLabel;
		QLineEdit *prop;
		function<void(float)> updateFunc;
};


class SimpleCheckBox
	: public CustomWidget
{
	private:
		SimpleCheckBox();

	public:
		SimpleCheckBox(const char* label, bool checked = false);
		~SimpleCheckBox() {};

		void SetLabelWidth(int width);
		void SetValue(bool checked);

		void OnUserEdit(function<void(bool)> func);
		bool GetValue();

	private:
		bool value;
		QLabel *qLabel;
		QCheckBox *prop;
		function<void(bool)> updateFunc;
};

class SimpleDropDown
	: public CustomWidget
{
	private:
		SimpleDropDown();

	public:
		SimpleDropDown(const char* label);
		~SimpleDropDown() {};

		void AddOption(const char* name, QVariant value);
		void RemoveOption(uint index);
		void RemoveOptions();
		void ResumeCallbacks();
		void StopCallbacks();

		void SetLabelWidth(int width);
		void SetIndex(unsigned int index);
		void SetValue(QVariant value);

		void OnChange(function<void(QVariant)> func);
		bool GetValue();

	private:
		bool update;
		unsigned int activeIndex;
		QLabel *qLabel;
		QComboBox *dropdown;
		function<void(QVariant)> updateFunc;
};