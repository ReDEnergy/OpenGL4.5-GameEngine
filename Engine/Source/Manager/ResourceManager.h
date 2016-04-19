#pragma once
#include <unordered_map>

#include <include/dll_export.h>
#include <include/pugixml.h>

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
		const std::unordered_map<std::string, Mesh*>& GetMeshList() const;

	private:
		void LoadMeshes(const pugi::xml_document &doc);
		void LoadPrefabObjects(const pugi::xml_document &doc);
		void LoadGameAudio(const pugi::xml_document &doc);

	private:
		std::unordered_map<std::string, GameObject*> _objects;
		std::unordered_map<std::string, unsigned int> _counter;
		std::unordered_map<std::string, Mesh*> _meshes;
};