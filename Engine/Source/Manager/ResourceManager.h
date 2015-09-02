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

		GameObject* GetGameObject(const char *name) const;
		GameObject * GetPropObject(const char * name) const;
		Mesh* GetMesh(const char *name) const;
		unsigned int GetGameObjectUID(const char *name);

		// TODO - move this to an outside lib
		void SetTransform(pugi::xml_node node, Transform &T);

	private:
		void LoadMeshes(const pugi::xml_document &doc);
		void LoadGameObjects(const pugi::xml_document &doc);
		void LoadGameAudio(const pugi::xml_document &doc);

	private:
		unordered_map<string, GameObject*> _objects;
		unordered_map<string, GameObject*> _props;
		unordered_map<string, unsigned int> _counter;
		unordered_map<string, Mesh*> _meshes;
};