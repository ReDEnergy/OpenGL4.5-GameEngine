#include "QtConfig.h"

#include <iostream>

#include <QIcon>
#include <QWidget>
#include <QFile>

using namespace std;

namespace RESOURCE {
	const string ICONS = "Resources/Icons/";
	const string STYLESHEETS = "Config/StyleSheets/";
}

string QtConfig::GetAppName()
{
	return "EnergyENGINE";
}

string QtConfig::GetAppVersion()
{
	return "0.1v";
}

QIcon* QtConfig::GetIcon(const char* fileName)
{
	return new QIcon((RESOURCE::ICONS + fileName).c_str());
}

bool QtConfig::SetWidgetStyleSheet(QWidget * widget, const char * file)
{
	QFile File((RESOURCE::STYLESHEETS + file).c_str());
	auto state = File.open(QFile::ReadOnly);
	if (state) {
		QString StyleSheet = QLatin1String(File.readAll());
		widget->setStyleSheet(StyleSheet);
		File.close();
		return true;
	}
	else {
		cout << "[Sylesheet file][Error] \t File \"" << RESOURCE::STYLESHEETS + file << "\" could not be oppened" << endl;
	}
	return false;
}