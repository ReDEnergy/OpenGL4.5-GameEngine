#pragma once
#include <unordered_map>

using namespace std;

#include <Editor/include/QTForward.h>

#include <string>

class QtConfig
{
	private:
		QtConfig() = delete;
		~QtConfig() = delete;

	public:
		static string GetAppName();
		static string GetAppVersion();
		static QIcon* GetIcon(const char * fileName);
		static bool SetWidgetStyleSheet(QWidget *widget, const char * file);
};
