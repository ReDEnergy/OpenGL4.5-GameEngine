#pragma once

#include <vector>
#include <functional>

#include <Editor/Windows/Interface/CustomWidget.h>

class Qlabel;
class QLineEdit;

// ***************************************************************************
// GLM UI component

class QtSortableInput
	: public CustomWidget
{
	public:
		QtSortableInput(const char* label, unsigned int precision = 2);
		~QtSortableInput() {};

		void SetLabelWidth(int width);
		void OnUserEdit(std::function<void(const std::vector<float>&)> func);
		void SetValue(const char* value);
		const std::vector<float>& GetValues() const;

	private:
		void Sort();

	private:
		std::vector<float> values;
		char *format;
		QLabel *qLabel;
		QLineEdit *prop;
		std::function<void(const std::vector<float>&)> updateFunc;
};