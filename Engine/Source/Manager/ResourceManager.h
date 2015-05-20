#pragma once
#include <unordered_map>

#include <include/dll_export.h>
#include <include/pugixml.h>

using namespace std;

class Mesh;
class Transform;
class GameObject;

namespace RESOURCE_PATH {
	const string ROOT = "Resources\\";
	const string AUDIO = ROOT + "Audio\\";
	const string MODELS = ROOT + "Models\\";
	const string TEXTURES = ROOT + "Textures\\";
	const string PHYSICS = ROOT + "Physics\\";
}

class DLLExport ResourceManager
{
	friend class SceneManager;

	protected:
		ResourceManager();
		~ResourceManager();

	public:
		void Load(const char *file);
		void LoadMeshes(const pugi::xml_document &doc);
		void LoadGameObjects(const pugi::xml_document &doc);
		void LoadGameAudio(const pugi::xml_document &doc);

		GameObject* GetGameObject(const char *name);
		unsigned int GetGameObjectUID(const char *name);

	public:
		void SetTransform(pugi::xml_node node, Transform &T);

	private:
		unordered_map<string, GameObject*> _objects;
		unordered_map<string, unsigned int> _counter;
		unordered_map<string, Mesh*> meshes;
};