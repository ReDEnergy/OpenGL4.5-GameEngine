#pragma once
#include <vector>

using namespace std;

#include <Core/World.h>
#include <Event/EventListener.h>

class Camera;
class CameraInput;
class CameraDebugInput;
class DirectionalLight;
class SpotLight;
class FrameBuffer;
class GameObject;
class Overlay;
class Player;
class SSAO;
class CSM;
class Texture;

class Game : public World,
			public EventListener
{
	public:
		Game();
		~Game();
		void Init();
		void Update(float elapsedTime, float deltaTime);

		void BarrelPhysicsTest();

	private:
		void OnEvent(EventType Event, Object *data);
		void InitSceneCameras();

	private:
		Camera				*activeCamera;
		Camera				*freeCamera;
		Camera				*gameCamera;
		CameraInput			*cameraInput;
		CameraDebugInput	*cameraDebugInput;

		DirectionalLight	*Sun;
		SpotLight			*Spot;

		FrameBuffer			*FBO;
		FrameBuffer			*FBO_Light;

		GameObject			*ScreenQuad;
		GameObject			*DebugPanel;
		Player				*player;

		SSAO				*ssao;
		CSM					*csm;

		vector<Camera*>		sceneCameras;
		unsigned int		activeSceneCamera;
};