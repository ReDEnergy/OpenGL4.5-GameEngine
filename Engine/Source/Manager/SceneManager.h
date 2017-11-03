#pragma once
#include <string>
#include <list>
#include <vector>
#include <functional>
#include <unordered_map>

#include <include/dll_export.h>
#include <include/pugixml.h>

class PointLight;
class GameObject;
class Camera;
class Shader;
class Texture;
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
		void FrustumCulling(Camera *gameCamera);
		void LightSpaceCulling(Camera *gameCamera, Camera *light);

		void AddObject(GameObject *obj);
		void AddPointLight(PointLight *light);
		void RemoveObject(GameObject *obj, bool destroy = false);
		GameObject* GetOrigin() const;

		void FrameEnded();

		// Very expensive if the scene is big
		// It should be called rarely and the output should be cached
		GameObject* FindGameObject(char *objectName) const;

		const std::list<GameObject*>& GetSceneObjects() const;
		const std::vector<GameObject*>& GetFrustrumObjects() const;
		const std::vector<PointLight*>& GetPointLights() const;

		Camera* GetActiveCamera();
		void SetActiveCamera(Camera *camera);

		void Render(Camera *camera);
		void RenderDeferredLights(Camera * camera, Texture * worldPositions, Texture * worldNormals) const;
		void PrepareSceneForRendering(Camera *camera);
		void OnPostRender(std::function<void(Camera& camera)> callback);

	private:
		void PrepareObjectForRendering(GameObject *object);

	private:
		Shader *R2T;
		Shader *R2TSk;

		const char *sceneFile;
		std::list<GameObject*> toAdd;
		std::list<GameObject*> toRemove;
		std::list<GameObject*> toDelete;
		std::unordered_map<std::string, std::list<GameObject*>> objectMap;
		Camera *activeCamera;
		GameObject *sceneOrigin;

		std::vector<PointLight*> pointLights;
		std::vector<GameObject*> frustumObjects;
		std::vector<GameObject*> normalRender;
		std::vector<GameObject*> skinnedRender;
		std::vector<GameObject*> alphaRender;

		std::list<std::function<void(Camera& camera)>> postRenderCallbacks;
};