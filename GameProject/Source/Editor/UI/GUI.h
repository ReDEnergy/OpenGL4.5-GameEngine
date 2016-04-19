#pragma once
#include <unordered_map>

using namespace std;

enum class QT_INSTACE {
	CODE_EDITOR_WINDOW,
	TEXT_PREVIEW,
	_3D_SCENE_WINDOW,
	FILE_PICKER,
	IMAGE_LIST,
	SCORE_TABLE,
	AVG_SCORE_TABLE,
	SCENE_EDITOR
};

class GUI
{
	private:
		GUI() {};
		~GUI() {};

	public:
		static void Init();

		static void Set(QT_INSTACE key, void* instace);

		template <class T>
		static T* Get(QT_INSTACE key);

	private:
		static unordered_map<QT_INSTACE, void*> instances;
};

template<class T>
inline T* GUI::Get(QT_INSTACE key)
{
	if (instances.find(key) != instances.end()) {
		return (T*)(instances[key]);
	}

	return nullptr;
}


