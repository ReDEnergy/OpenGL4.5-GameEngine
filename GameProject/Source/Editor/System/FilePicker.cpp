#include "FilePicker.h"

#include <iostream>

using namespace std;

FilePicker::FilePicker()
{
}

void FilePicker::OpenForSave()
{
	reason = Reason::SAVE;
	setFileMode(QFileDialog::AnyFile);
	setAcceptMode(QFileDialog::AcceptSave);
	exec();
}

void FilePicker::OpenForLoad()
{
	reason = Reason::LOAD;
	setFileMode(QFileDialog::ExistingFile);
	setAcceptMode(QFileDialog::AcceptOpen);
	exec();
}

bool FilePicker::IsSaving() const
{
	return (reason == Reason::SAVE);
}
