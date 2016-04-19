#pragma once
#include <pch.h>
#include <vector>

using namespace std;

class Game : 
	public World,
	public EventListener,
	public ObjectInput
{
	public:
		Game();
		~Game();
		void Init();
		void InitUIHooks();

		void FrameStart();
		void Update(float deltaTime);
		void FrameEnd();

		void BarrelPhysicsTest(bool pointLights);

	private:
		void OnEvent(EventType Event, void *data);
		void OnEvent(const string& eventID, void *data);
		void InitSceneCameras();

	private:
		Camera				*freeCamera;
		Camera				*gameCamera;
		CameraInput			*cameraInput;
		CameraDebugInput	*cameraDebugInput;

		DirectionalLight	*Sun;
		SpotLight			*Spot;

		FrameBuffer			*FBO;
		FrameBuffer			*FBO_Out;
		FrameBuffer			*FBO_Light;

		GameObject			*ScreenQuad;
		GameObject			*DebugPanel;

		SSAO				*ssao;
		Texture				*ShadowMap;

		vector<Camera*>		sceneCameras;
		unsigned int		activeSceneCamera;

		ProfileTimer		*cpuTime;
};