#pragma once
#include <Editor/include/QTForward.h>

using namespace std;

class QtTextValidator
{
	public:
		static QRegExpValidator* GetFloatValidator();

	private:
		static QRegExpValidator* floatValidator;
};
