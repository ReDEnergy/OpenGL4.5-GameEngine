#pragma once
#include <pch.h>
#include <vector>

#include <Runtime/Modules/RuntimeGLModule.h>

class Game :
	public RuntimeGLModule,
	public EventListener
{
	public:
		Game();
		~Game();
		void Init();
		void InitUIHooks();

		void FrameStart();
		void Update(double deltaTime);
		void FrameEnd();

		void BarrelPhysicsTest(bool pointLights);
		static FrameBuffer * GetOffScreenBuffer();

	private:
		void Render();
		void RenderScene(Camera *camera) const;
		void FinishComposition(FrameBuffer *outFBO, Camera *camera) const;

		// Input options
		void OnInputUpdate(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);

		// Events
		void OnEvent(EventType Event, void *data);
		void OnEvent(const std::string& eventID, void *data);
		void InitSceneCameras();

		void ToggleRenderingToOculusRift();

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

		static FrameBuffer	*FBO_offscreen;

		#ifdef OCULUS_RIFT_HMD
		FrameBuffer			*FBO_OutUI;
		#endif

		GameObject			*screenQuad;

		SSAO				*ssao;
		Texture				*ShadowMap;

		std::vector<Camera*>	sceneCameras;
		unsigned int			activeSceneCamera;

		ProfilingTimer		*cpuTime;
};