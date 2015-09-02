#pragma once
#include <list>
#include <vector>

#include <include/dll_export.h>
#include <include/pugixml.h>

class PointLight;
class GameObject;
class Camera;
class DirectionalLight;

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
		void LightSpaceCulling(Camera *camera, DirectionalLight *light);
		void AddObject(GameObject *obj);
		void RemoveObject(GameObject *obj, bool destroy = false);
		void FrameEnded();
		GameObject* GetGameObject(char *refID, unsigned int instanceID);
		Camera* GetActiveCamera();

		void SetActiveCamera(Camera *camera);

	private:
		const char *sceneFile;
		list<GameObject*> toAdd;
		list<GameObject*> toRemove;
		list<GameObject*> toDelete;
		Camera *activeCamera;

	public:
		vector<PointLight*> lights;
		list<GameObject*> activeObjects;
		list<GameObject*> frustumObjects;

};