#pragma once
#include <list>
#include <vector>
#include <functional>

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
		const std::list<GameObject*>& GetSceneObjects() const;
		const std::list<GameObject*>& GetFrustrumObjects() const;
		const std::vector<PointLight*>& GetPointLights() const;

		Camera* GetActiveCamera();
		void SetActiveCamera(Camera *camera);

		void Render(Camera *camera);
		void OnPostRender(std::function<void(Camera& camera)> callback);

	private:
		Shader *R2T;
		Shader *R2TSk;

		const char *sceneFile;
		std::list<GameObject*> toAdd;
		std::list<GameObject*> toRemove;
		std::list<GameObject*> toDelete;
		Camera *activeCamera;

		std::vector<PointLight*> lights;
		std::list<GameObject*> sceneObjects;
		std::list<GameObject*> frustumObjects;


		std::list<std::function<void(Camera& camera)>> postRenderCallbacks;
};