#include "Utils.h"

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QRegExpValidator>

QRegExpValidator* QtTextValidator::floatValidator = nullptr;

QRegExpValidator * QtTextValidator::GetFloatValidator()
{
	if (floatValidator == nullptr) {
		QRegExp rx("(-?[0-9]*\\.*[0-9]*)");
		floatValidator = new QRegExpValidator(rx);
	}
	return floatValidator;
}
