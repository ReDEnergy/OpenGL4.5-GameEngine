#pragma once

#include <vector>
#include <string>
#include <functional>
#include <iostream>

#include <Editor/Utils/Utils.h>
#include <Editor/Windows/Interface/CustomWidget.h>

#include <QLabel>
#include <QLineEdit>
#include <QValidator>

#include <include/glm.h>
#include <include/math.h>

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

			setProperty("GLMVecComponent", vec.length());

			qtLayout->setDirection(QBoxLayout::LeftToRight);
			qtLayout->setSpacing(0);

			{
				auto l = new QLabel(label);
				l->setProperty("Name", "");
				l->setMinimumWidth(65);
				l->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
				qtLayout->addWidget(l);
			}

			char const *format[4] = { "x", "y", "z", "w" };

			for (int i = 0; i < vec.length(); i++)
			{
				auto l = new QLabel(format[i]);
				l->setProperty("Component", format[i]);
				l->setMinimumWidth(15);
				l->setAlignment(Qt::AlignCenter);
				prop[i] = new QLineEdit();
				prop[i]->setMinimumWidth(40);
				prop[i]->setValidator(QtTextValidator::GetFloatValidator());
				qtLayout->addWidget(l);
				qtLayout->addWidget(prop[i]);

				QObject::connect(prop[i], &QLineEdit::editingFinished, this, [&, i](){
					value[i] = prop[i]->text().toFloat();
					if (updateFunc)
						updateFunc(value);
				});
			}
		}

		~GLMVecComponent() {};

		void OnUserEdit(std::function<void(T)> func) {
			updateFunc = func;
		}

		void SetValue(const T &vec)
		{
			if (value == vec)
				return;
			value = vec;
			char buff[20];
			for (int i = 0; i < vec.length(); i++)
			{
				sprintf(buff, "%.3f", vec[i]);
				prop[i]->setText(buff);
			}
		}

	private:
		T value;
		std::function<void(T)> updateFunc;
		QLineEdit *prop[4];
};
