#pragma once
#include <Editor/include/QtForward.h>

class QtTextValidator
{
	public:
		static QRegExpValidator* GetFloatValidator();

	private:
		static QRegExpValidator* floatValidator;
};
