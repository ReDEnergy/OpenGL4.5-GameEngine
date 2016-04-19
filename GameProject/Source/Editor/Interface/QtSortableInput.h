#pragma once

#include <vector>
#include <Editor/Windows/Interface/CustomWidget.h>
#include <functional>

class Qlabel;
class QLineEdit;

using namespace std;

// ***************************************************************************
// GLM UI component

class QtSortableInput
	: public CustomWidget
{
	public:
		QtSortableInput(const char* label, unsigned int precision = 2);
		~QtSortableInput() {};

		void SetLabelWidth(int width);
		void OnUserEdit(function<void(const vector<float>&)> func);
		void SetValue(const char* value);
		const vector<float>& GetValues() const;

	private:
		void Sort();

	private:
		vector<float> values;
		char *format;
		QLabel *qLabel;
		QLineEdit *prop;
		function<void(const vector<float>&)> updateFunc;
};