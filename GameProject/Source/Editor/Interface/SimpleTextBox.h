#pragma once

#include <string>
#include <iostream>
#include <functional>

#include <Editor/Windows/Interface/CustomWidget.h>

using namespace std;

class SimpleTextBox
	: public CustomWidget
{
	public:
		SimpleTextBox(const char* label, const char* value = nullptr, bool readOnly = false);

	public:
		void SetReadOnly(bool value);
		void SetValue(const char * value);
		void OnUserEdit(function<void(const char*)> func);
		const char* GetValue() const;

	private:
		QLabel *qLabel;
		QLineEdit *prop;
		function<void(const char*)> updateFunc;
};

