#pragma once
#include <list>
#include <vector>

#include <include/dll_export.h>
#include <include/pugixml.h>

class PointLight;
class GameObject;
class Camera;
class Shader;
class DirectionalLight;

class DLLExport SceneManager
{
	protected:
		SceneManager();
		~SceneManager();

	public:
		void Init();
		void LoadScene(const char *fileName);
		void ReloadScene();
			 
		void GlobalUpdate();
		void Update();
		void FrustumCulling(Camera *camera);
		void LightSpaceCulling(Camera *camera, DirectionalLight *light);

		void AddObject(GameObject *obj);
		void AddPointLight(PointLight *light);
		void RemoveObject(GameObject *obj, bool destroy = false);

		void FrameEnded();

		GameObject* GetGameObject(char *refID, unsigned int instanceID);
		const std::list<GameObject*>& GetActiveObjects() const;
		const std::list<GameObject*>& GetFrustrumObjects() const;
		const std::vector<PointLight*>& GetPointLights() const;

		Camera* GetActiveCamera();
		void SetActiveCamera(Camera *camera);

		void Render(Camera *camera);

	private:
		Shader *R2T;
		Shader *R2TSk;

		const char *sceneFile;
		std::list<GameObject*> toAdd;
		std::list<GameObject*> toRemove;
		std::list<GameObject*> toDelete;
		Camera *activeCamera;

		std::vector<PointLight*> lights;
		std::list<GameObject*> activeObjects;
		std::list<GameObject*> frustumObjects;

};