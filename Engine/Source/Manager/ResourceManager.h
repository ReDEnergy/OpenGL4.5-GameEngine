#pragma once
#include <unordered_map>

#include <include/dll_export.h>
#include <include/pugixml.h>

using namespace std;

class Mesh;
class Transform;
class GameObject;

class DLLExport ResourceManager
{
	friend class SceneManager;

	protected:
		ResourceManager();
		~ResourceManager();

	public:
		void Load(const char *file);
		GameObject* GetGameObject(const char *name);

	public:
		void SetTransform(pugi::xml_node node, Transform &T);

	private:
		unordered_map<string, GameObject*> _objects;
		unordered_map<string, Mesh*> meshes;
};