#pragma once
#include <list>
#include <vector>

#include <include/dll_export.h>
#include <include/pugixml.h>

class PointLight;
class GameObject;
class Camera;

using namespace std;

class DLLExport SceneManager
{
	protected:
		SceneManager();
		~SceneManager();

	public:
		void LoadScene(const char *fileName);
		void ReloadScene();
			 
		void Update();
		void FrustumCulling(Camera *camera);
		void AddObject(GameObject *obj);
		void RemoveObject(GameObject *obj);

	private:
		const char *sceneFile;
		list<GameObject*> toAdd;
		list<GameObject*> toRemove;

	public:
		vector<PointLight*> lights;
		list<GameObject*> activeObjects;
		list<GameObject*> frustumObjects;

};