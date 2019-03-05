#pragma once

#include <vector>

class Mesh;
class Shader;
class Camera;
class Transform;
class GameObject;
class SimpleColorPicking;

#include <Core/World.h>
#include <Component/ObjectInput.h>

class SimpleScene : public World, private ObjectInput
{
	public:
		SimpleScene();
		~SimpleScene();

		void InitResources(WindowObject *window);

		void RenderScene() const;
		void DrawCoordinatSystem() const;
		void SetDrawGrid(bool value);

		void AddGameObject(GameObject *go);
		void RemoveGameObject(GameObject *go);

		Shader *GetStandardShader() const;
		Shader *GetScreenShader() const;

		void ReloadShaders();

		void FrameStart() override;
		void Update(float deltaTime) override;

	protected:
		virtual void OnKeyPress(int key, int mods) override;

	public:
		Camera *camera;
		ObjectInput *cameraInput;

		GameObject *box;
		GameObject *screenQuad;

	private:
		SimpleColorPicking *colorPicking;
		Shader *screenShader;
		Shader *sceneShader;
		Shader *axisShader;
		GameObject *xozPlane;
		GameObject *coordinateSystem;
		std::vector<GameObject*> gameObjects;

		int locColor;
};