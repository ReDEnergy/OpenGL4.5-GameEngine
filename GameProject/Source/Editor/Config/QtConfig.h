#pragma once
#include <unordered_map>

#include <Editor/include/QtForward.h>

#include <string>

class QtConfig
{
	private:
		QtConfig() = delete;
		~QtConfig() = delete;

	public:
		static std::string GetAppName();
		static std::string GetAppVersion();
		static QIcon* GetIcon(const char * fileName);
		static bool SetWidgetStyleSheet(QWidget *widget, const char * file);
};
