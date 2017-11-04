#include <pch.h>
#include "Game.h"

#include <include/gl.h>

#include <GUI/Channels/ObjectPropertiesUIChannel.h>

#include <Game/Input/AnimationInput.h>
#include <Game/Input/SimpleObjectControl.h>
#include <Prototyping/RotationConstraints/RotationConstraints.h>

#include <templates/Singleton.h>

#ifdef NDEBUG
// RELEASE BUILD
#endif
#ifdef _DEBUG
// DEBUG BUILD
#endif

#include <SkeletalSystem/AnimationSystem.h>
AnimationSystem *animationSystem = nullptr;

#undef KINECT_SENSOR
#ifdef KINECT_SENSOR
#include <Kinect/KinectSensor.h>
#include <Kinect/KinectSkeletalTracking.h>
#include <Kinect/Depth/KinectPointCloud.h>
#include <Kinect/KinectStreaming.h>

KinectSensor *kinectSensor = nullptr;
KinectStreaming *kinectStreaming = nullptr;
KinectPointCloud *kinectPointCloud = nullptr;
KinectSkeletalTracking *kinectSkeletalTracking = nullptr;
#endif

//#undef LEAP_SENSOR
#ifdef LEAP_SENSOR
#include <LeapMotion/LeapSensor.h>
#include <LeapMotion/ControlPanel/LeapControlpanel.h>
LeapSensor *leapSensor = nullptr;
LeapControlPanel *leapControlPanel = nullptr;
#endif

//#undef OCULUS_RIFT_HMD
#ifdef OCULUS_RIFT_HMD
#include <OculusRift/OculusRiftHMD.h>
OculusRiftHMD *oculusHMD = nullptr;
#endif

#define GAME_MODULES
#ifdef GAME_MODULES
#include <MotionSystem/MotionSystem.h>
MotionSystem *motionSystem = nullptr;
#endif

// Add support for PhysX Engine
#ifdef PHYSX_ENGINE
#endif

//#define SOV_CAMERA_BOUNDING_BOX
#ifdef SOV_CAMERA_BOUNDING_BOX
	bool updateBoundingBox = true;
#endif

using namespace std;

Game::Game()
{
	Configure("Game");
	SetUIUpdateFrequency(0);

	window = WindowManager::GetDefaultWindow();
	window->props.hideOnClose = true;
	window->SetVSync(true);
	AttachTo(window);

	SubscribeTo("ToggleGameWindow", [this](void *data) {
		window->props.visible ? window->Hide() : window->Show();
		if (window->props.visible) {
			window->SetSize(1280, 720);
			window->CenterWindow();
		}
	});

	SubscribeTo("V-Sync-Toggle", [this](void *data) {
		window->ToggleVSync();
	});

	SubscribeTo("OculusRift-ToggleRendering", [this](void *data) {
		ToggleRenderingToOculusRift();
	});
}

Game::~Game() {
}

void Game::Init()
{
	window->MakeCurrentContext();

	cpuTime = new ProfileTimer();
	cpuTime->SetMessage("CPU time");

	// Game resolution
	glm::ivec2 resolution = window->GetResolution();
	float aspectRation = float(resolution.x) / resolution.y;

	// Cameras
	gameCamera = new Camera();
	gameCamera->SetName("Game Camera");
	gameCamera->SetPerspective(60, aspectRation, 0.001f, 300);
	gameCamera->SetPosition(glm::vec3(0, 3, 3));
	gameCamera->transform->SetMoveSpeed(2);
	Manager::GetScene()->AddObject(gameCamera);

	freeCamera = new Camera();
	freeCamera->SetName("Free Camera");
	freeCamera->SetPerspective(40, aspectRation, 0.001f, 300);
	freeCamera->SetPosition(glm::vec3(0.0f, 10.0f, 10.0f));
	freeCamera->transform->SetWorldRotation(glm::vec3(-35, 0, 0));
	freeCamera->Update();
	Manager::GetScene()->AddObject(freeCamera);

	Manager::GetScene()->SetActiveCamera(gameCamera);

	// Lights
	Sun = new DirectionalLight();
	Sun->SetName("Directional Light");
	Sun->transform->SetWorldPosition(glm::vec3(0.0f, 5.0f, 0.0f));
	Sun->transform->SetWorldRotation(glm::vec3(-35, -45, 0));
	Sun->SetOrthgraphic(40, 40, 0.1f, 200);
	Sun->SetCamera(gameCamera);
	Sun->Update();
	Manager::GetScene()->AddObject(Sun);

	Spot = new SpotLight();
	Spot->SetPosition(glm::vec3(5.25, 3, 9));
	Spot->SetPerspective(90, 1, 0.1f, 50);
	Spot->transform->SetWorldRotation(glm::vec3(-45, -25, 0));
	Manager::GetDebug()->Add(Spot);
	Manager::GetScene()->AddObject(Spot);

	ShadowMap = new Texture();
	ShadowMap->Create2DTexture(resolution.x, resolution.y, 4, 32, GL_FLOAT);

	// --- Create FBO for rendering to texture --- //
	FBO = new FrameBuffer();
	FBO->Generate(resolution.x, resolution.y, 5);

	FBO_Light = new FrameBuffer();
	FBO_Light->Generate(resolution.x, resolution.y, 1, false);

	FBO_Out = new FrameBuffer();
	FBO_Out->Generate(resolution.x, resolution.y, 1, false);
	FrameBuffer::SetOffScreenBuffer(FBO_Out);

	FBO_OutUI = new FrameBuffer();
	FBO_OutUI->Generate(resolution.x, resolution.y, 1, false);

	// Rendering 
	ssao = new SSAO();
	ssao->Init(resolution.x, resolution.y);

	screenQuad = Manager::GetResource()->GetGameObject("render-quad");
	screenQuad->Update();

	// --- Debugging --- //
	auto *TDBG = Manager::GetTextureDebugger();
	TDBG->SetChannel(0, FBO);
	TDBG->PushToChannel(0, FBO_Light->GetTexture(0));
	TDBG->PushToChannel(0, FBO_Out->GetTexture(0));
	TDBG->SetChannel(1, Manager::GetPicker()->FBO);
	TDBG->PushToChannel(1, Manager::GetPicker()->FBO_Gizmo->GetTexture(0));
	TDBG->SetChannel(3, Sun->GetTextureBuffer());

	cameraInput = new CameraInput(Manager::GetScene()->GetActiveCamera());
	cameraDebugInput = new CameraDebugInput(gameCamera);
	ObjectInput *DI = new DebugInput();
	ObjectInput *EI = new EditorInput();
	cameraInput->AttachTo(window);
	cameraDebugInput->AttachTo(window);
	DI->AttachTo(window);
	EI->AttachTo(window);

	// GameObjects
	auto ground = Manager::GetScene()->FindGameObject("ground");
	ground->SetSelectable(false);

	//for (uint i = 0; i < 70; i++) {
	//	GameObject *tree = Manager::GetResource()->GetGameObject("bamboo");
	//	tree->SetName(string(tree->GetName()) + "_" + to_string(i));
	//	ground->AddChild(tree);
	//	tree->transform->SetWorldPosition(glm::vec3(rand() % 100 - 50, 0, rand() % 100 - 50));
	//	tree->transform->SetScale(glm::vec3(0.1f));
	//}

	InitSceneCameras();

	Manager::GetScene()->Update();

	// Kinect Modules
	#ifdef KINECT_SENSOR

	kinectSensor = SINGLETON(KinectSensor);
	kinectSkeletalTracking = SINGLETON(KinectSkeletalTracking);
	kinectSkeletalTracking->Init(kinectSensor);

	//kinectStreaming = SINGLETON(KinectStreaming);
	//kinectStreaming->Init(kinectSensor);
	//kinectPointCloud = new KinectPointCloud();
	#endif

	#ifdef OCULUS_RIFT_HMD
	oculusHMD = SINGLETON(OculusRiftHMD);
	auto success = oculusHMD->Connect();
	if (success) {
		oculusHMD->SetRenderToHMD(false);
		auto device = oculusHMD->GetGameObjectDevice();

		auto box = Manager::GetResource()->GetGameObject("box");
		box->SetName("OculusRift Origin");
		box->transform->SetScale(glm::vec3(0.1f));
		box->transform->SetWorldPosition(glm::vec3(0, 0.5f, -3));
		box->AddChild(device);
		Manager::GetScene()->AddObject(box);

		sceneCameras.push_back(oculusHMD->GetEyeCamera(0));
		sceneCameras.push_back(oculusHMD->GetEyeCamera(1));
	}

	SubscribeToEvent("OculusRift-ToggleRendering");
	#endif

	#ifdef LEAP_SENSOR
	leapSensor = SINGLETON(LeapSensor);
	leapSensor->Connect();
	leapControlPanel = new LeapControlPanel();
	#endif

	// SkeletalSystem
	animationSystem = SINGLETON(AnimationSystem);


	#ifdef GAME_MODULES
	motionSystem = SINGLETON(MotionSystem);
	#endif

	auto rotationConstraintsTest = new RotationConstraints();

	// Listens to Events and Input
	SubscribeToEvent(EventType::SWITCH_CAMERA);
	SubscribeToEvent(EventType::FRONT_END_INITIALIZED);

	#ifdef PHYSICS_ENGINE

		SubscribeTo("UsePhysics", [this](void *data) {
			auto obj = static_cast<GameObject*>(data);
			if (obj->physics == nullptr) obj->physics = new Physics(obj);
			obj->physics->SetRigidBody();
		});

		// Bind UI callbacks for Dynamic target
		auto setGravity = [this](void *data, bool state) {
			auto obj = static_cast<GameObject*>(data);
			if (obj->physics) {
				obj->physics->SetGravity(state);
			}
		};
		auto setGravityTrue = std::bind(setGravity, std::placeholders::_1, true);
		auto setGravityFalse = std::bind(setGravity, std::placeholders::_1, false);

		SubscribeTo("SetGravityTrue", setGravityTrue);
		SubscribeTo("SetGravityFalse", setGravityFalse);

		// Bind UI callbacks for Dynamic target
		auto makeDynamic = [this](void *data, bool state) {
			auto obj = static_cast<GameObject*>(data);
			if (obj->physics) {
				obj->physics->SetIsDynamic(state);
			}
		};
		auto setDynamicTrue = std::bind(makeDynamic, std::placeholders::_1, true);
		auto setDynamicFalse = std::bind(makeDynamic, std::placeholders::_1, false);

		SubscribeTo("SetIsDynamicTrue", setDynamicTrue);
		SubscribeTo("SetIsDynamicFalse", setDynamicFalse);

		// Bind UI callbacks  for Kinematic target
		auto makeKinematic = [this](void *data, bool state) {
			auto obj = static_cast<GameObject*>(data);
			if (obj->physics) {
				obj->physics->SetIsKinematic(state);
			}
		};
		auto kinematicTrue = std::bind(makeKinematic, std::placeholders::_1, true);
		auto kinematicFalse = std::bind(makeKinematic, std::placeholders::_1, false);

		SubscribeTo("SetIsKinematicTrue", kinematicTrue);
		SubscribeTo("SetIsKinematicFalse", kinematicFalse);

		// Bind UI callbacks for Trigger target
		auto makeTriggerZone = [this](void *data, bool state) {
			auto obj = static_cast<GameObject*>(data);
			if (obj->physics) {
				obj->physics->SetIsTrigger(state);
			}
		};
		auto setTriggerZoneTrue = std::bind(makeTriggerZone, std::placeholders::_1, true);
		auto setTriggerZoneFalse = std::bind(makeTriggerZone, std::placeholders::_1, false);

		SubscribeTo("SetIsTriggerTrue", setTriggerZoneTrue);
		SubscribeTo("SetIsTriggerFalse", setTriggerZoneFalse);

		// Bind UI callback for setting physics mass
		auto setPhysicsMass = [this](void *data) {
			auto chn = static_cast<ObjectPropertiesUIChannel*>(data);
			auto obj = chn->context;
			if (obj->physics) {
				obj->physics->SetMass(chn->physicsMass);
			}
		};
		SubscribeTo("SetPhysicsMass", setPhysicsMass);

	#endif
}

void Game::InitUIHooks()
{
	Manager::GetEvent()->EmitSync("Set-Profile-Timer", cpuTime);
}

void Game::FrameStart()
{
	window->MakeCurrentContext();
	Manager::GetEvent()->EmitSync(EventType::FRAME_START);
}

void Game::Update(double deltaTime)
{
	FrameStart();

	if (Engine::GetElapsedTime() - cpuTime->GetStartTime() > 0.5) {
		cpuTime->Reset();
		cpuTime->Start();
	}

	Camera *activeCamera = Manager::GetScene()->GetActiveCamera();

	// -----------------------//
	// --- Sensors Update --- //
	// -----------------------//

	#ifdef KINECT_SENSOR
	kinectSensor->Update();
	kinectSkeletalTracking->Update();
	//kinectStreaming->Update();
	//kinectPointCloud->Update();
	cpuTime->Lap("Kinect");
	#endif

	#ifdef OCULUS_RIFT_HMD
	oculusHMD->GetLoopInfo();
	cpuTime->Lap("OculusRift");
	#endif

	#ifdef LEAP_SENSOR
	leapSensor->Update();
	leapControlPanel->Update();
	cpuTime->Lap("LeapMotion");
	#endif

	animationSystem->Update();
	cpuTime->Lap("AnimationSystem");

	#ifdef GAME_MODULES
	motionSystem->Update();
	#endif

	///////////////////////////////////////////////////////////////////////////
	// Update Scene

	Manager::GetAudio()->Update(activeCamera);
	cpuTime->Lap("Scene audio");

	Manager::GetEvent()->Update();
	cpuTime->Lap("Scene events");

	Manager::GetScene()->Update();
	cpuTime->Lap("Scene update");

	Manager::GetScene()->PrepareSceneForRendering(activeCamera);
	cpuTime->Lap("Scene culling");

	#ifdef SOV_CAMERA_BOUNDING_BOX
	if (updateBoundingBox) {
		Manager::GetScene()->LightSpaceCulling(gameCamera, gameCamera);
	}
	#else
	//Manager::GetScene()->LightSpaceCulling(gameCamera, Sun);
	//Sun->transform->ClearMotionState();
	#endif

	//Manager::GetPicker()->DrawSceneForPicking();
	Manager::GetEvent()->EmitSync(EventType::FRAME_UPDATE);

	cpuTime->Lap("Scene Update");

	///////////////////////////////////////////////////////////////////////////
	// Start Physics Simulation with scene rendering

	#ifdef PHYSICS_ENGINE
	bool asyncPhysics = true;
	Manager::GetPhysics()->GetPhysX()->StepPhysics(static_cast<float>(deltaTime));
	if (!asyncPhysics) {
		Manager::GetPhysics()->GetPhysX()->FetchResults();
	}
	cpuTime->Lap("Physics");
	#endif


	///////////////////////////////////////////////////////////////////////////
	// Scene Rendering
	Render();

	///////////////////////////////////////////////////////////////////////////
	// Finish Physics Simulation
	#ifdef PHYSICS_ENGINE
	if (asyncPhysics) {
		Manager::GetPhysics()->GetPhysX()->FetchResults();
	}
	#endif

	FrameEnd();
}

void Game::Render()
{
	auto activeCamera = Manager::GetScene()->GetActiveCamera();

	#ifdef OCULUS_RIFT_HMD
	// ---   Output to HMD if available   --- //
	if (oculusHMD->IsRenderingToHMD())
	{
		auto eyeCamera = oculusHMD->GetEyeCamera(0);
		if (eyeCamera) {
			Manager::GetScene()->SetActiveCamera(eyeCamera);
			RenderScene(eyeCamera);
			oculusHMD->SumbitEyeFrame(0, FBO_Out->GetTexture(0));
			cpuTime->Lap("OculusVR - Left Eye");
		}

		eyeCamera = oculusHMD->GetEyeCamera(1);
		if (eyeCamera) {
			Manager::GetScene()->SetActiveCamera(eyeCamera);
			RenderScene(eyeCamera);
			oculusHMD->SumbitEyeFrame(1, FBO_Out->GetTexture(0));
			cpuTime->Lap("OculusVR - Right Eye");
		}

		Manager::GetScene()->SetActiveCamera(activeCamera);
	}
	else {
	#endif

		RenderScene(activeCamera);
		Manager::GetDebug()->Render(activeCamera);

	#ifdef OCULUS_RIFT_HMD
	}
	#endif
}

void Game::RenderScene(Camera *camera) const
{
	OPENGL_RAII_LOCK();
	Manager::GetDebug()->Render(camera);
	Manager::GetPicker()->Update(camera);

	FBO->Bind(true);

	Manager::GetScene()->Render(camera);

	#ifdef KINECT_SENSOR
	kinectSkeletalTracking->Render();
	#endif

	cpuTime->Lap("Scene Rendering");

	///////////////////////////////////////////////////////////////////////////
	// Shadows Casting
	{
		//Sun->CastShadows();
		//Spot->CastShadows();
		//cpuTime->Lap("Shadow Casting");
		//Sun->BakeShadows(FBO);
		//Spot->BakeShadows(FBO);
		//cpuTime->Lap("Shadow Backing");
	}

	// ---------------------------//
	// --- Deferred Rendering --- //
	// ---------------------------//

	if (!Manager::GetRenderSys()->Is(RenderState::FORWARD)) {

		// --- Deferred Lighting --- //
		FBO_Light->Bind();
		Manager::GetScene()->RenderDeferredLights(camera, FBO->GetTexture(1), FBO->GetTexture(2));
		cpuTime->Lap("Deferred Lighing");

		// --- Screen Space Ambient Occlusion (SSAO) --- //
		if (Manager::GetRenderSys()->Is(RenderState::SS_AO)) {
			ssao->Update(FBO, camera);
			cpuTime->Lap("SSAO");
		}
	}

	FinishComposition(FBO_Out, camera);
}

void Game::FinishComposition(FrameBuffer *outFBO, Camera *camera) const
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	// ---   Composition step   --- //
	{
		outFBO->Bind();
		auto rez = outFBO->GetResolution();

		Shader *Composition = Manager::GetShader()->GetShader("composition");
		Composition->Use();
		glUniform1i(Composition->loc_debug_view, Manager::GetDebug()->GetActiveState());
		glUniform1i(Composition->active_ssao, Manager::GetRenderSys()->Is(RenderState::SS_AO));
		glUniform1i(Composition->active_selection, Manager::GetPicker()->HasActiveSelection());
		camera->BindProjectionDistances(Composition);

		FBO->BindTexture(0, GL_TEXTURE0);
		FBO_Light->BindTexture(0, GL_TEXTURE1);
		ShadowMap->BindToTextureUnit(GL_TEXTURE2);
		ssao->BindTexture(GL_TEXTURE3);
		FBO->BindDepthTexture(GL_TEXTURE4);
		Manager::GetDebug()->FBO->BindTexture(0, GL_TEXTURE5);
		Manager::GetDebug()->FBO->BindDepthTexture(GL_TEXTURE6);
		Manager::GetPicker()->FBO_Gizmo->BindTexture(0, GL_TEXTURE7);

		screenQuad->Render(Composition);
	}

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	cpuTime->Lap("Composition");
}

void Game::FrameEnd()
{
	// ---   Final output step   --- //
	if (window->props.visible)
	{
		FrameBuffer::Unbind(window->GetResolution(), true);
		Shader *screen = Manager::GetShader()->GetShader("screen");
		screen->Use();
		FBO_Out->BindTexture(0, GL_TEXTURE0);
		screenQuad->Render(screen);
	}
	else {
		FBO_Out->Bind(false);
	}

	Manager::GetDebugText()->Render();
	Manager::GetTextureDebugger()->Render();
	Manager::GetMenu()->RenderMenu();
	cpuTime->Lap("Overlays");

	Manager::GetEvent()->EmitSync(EventType::FRAME_END);
	Manager::GetScene()->FrameEnded();

	cpuTime->Stop();

	#ifdef OCULUS_RIFT_HMD
	if (oculusHMD->IsRenderingToHMD())
		oculusHMD->FinishFrame();
	#endif
}

void Game::BarrelPhysicsTest(bool pointLights)
{
	#ifdef PHYSICS_ENGINE
	glm::vec3 pos = gameCamera->transform->GetWorldPosition();
	GameObject *barrel = Manager::GetResource()->GetGameObject("box");
	for (int i=0; i<100; i++) {
		if (pointLights) {
			PointLight *obj = new PointLight(*barrel);
			obj->transform->SetWorldPosition(pos); // +glm::vec3(rand() % 10 - 5, rand() % 5 + 5, rand() % 10 - 5);
			Manager::GetScene()->AddPointLight(obj);
			Manager::GetScene()->AddObject(obj);
		}
		else {
			GameObject *obj = new GameObject(*barrel);
			obj->transform->SetWorldPosition(glm::vec3(rand() % 10 - 5, rand() % 5 + 5, rand() % 10 - 5));
			Manager::GetScene()->AddObject(obj);
		}
	}
	#endif
}

void Game::OnInputUpdate(float deltaTime, int mods) {

	Camera *activeCamera = Manager::GetScene()->GetActiveCamera();

	if (window->KeyHold(GLFW_KEY_RIGHT))
		activeCamera->MoveRight(deltaTime);
	if (window->KeyHold(GLFW_KEY_UP))
		activeCamera->MoveForward(deltaTime);
	if (window->KeyHold(GLFW_KEY_LEFT))
		activeCamera->MoveRight(-deltaTime);
	if (window->KeyHold(GLFW_KEY_DOWN))
		activeCamera->MoveBackward(deltaTime);

	activeCamera->Update();
}

void Game::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_ESCAPE) {
		Manager::GetEvent()->EmitSync(EventType::OPEN_GAME_MENU, nullptr);
		return;
	}

	if (key == GLFW_KEY_F8) {
		Manager::GetAudio()->PlaySoundFX("bell");
		return;
	}

	#ifdef OCULUS_RIFT_HMD
	if (mods == 0) {
		if (key == GLFW_KEY_O) {
			ToggleRenderingToOculusRift();
		}

		if (key == GLFW_KEY_L) {
			static int switchEye = true;
			FrameBuffer::SetOffScreenBuffer(switchEye ? FBO_OutUI : FBO_Out);
			switchEye = !switchEye;
			return;
		}

		if (key == GLFW_KEY_P) {
			oculusHMD->RecenterHMD();
			return;
		}
	}
	#endif

#ifdef SOV_CAMERA_BOUNDING_BOX
	if (key == GLFW_KEY_F3) {
		updateBoundingBox = !updateBoundingBox;
		return;
	}

	if (key == GLFW_KEY_F4) {
		auto state = Manager::GetDebug()->GetBoundingBoxMode();
		if (state == DebugInfo::BBOX_MODE::OBJECT_SAPCE)
			Manager::GetDebug()->SetBoundingBoxMode(DebugInfo::BBOX_MODE::CAMERA_SPACE);
		else 
			Manager::GetDebug()->SetBoundingBoxMode(DebugInfo::BBOX_MODE::OBJECT_SAPCE);
		return;
	}
#endif

	if (mods == (GLFW_MOD_SHIFT + GLFW_MOD_CONTROL))
	{
		if (key == GLFW_KEY_B) {
			Manager::GetEvent()->EmitSync("barrels-light", nullptr);
		}

		if (key == GLFW_KEY_N) {
			Manager::GetEvent()->EmitSync("barrels", nullptr);
		}
	}
}

void Game::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	//	cout << mouseY << "-" << mouseX << endl;
}

void Game::OnEvent(EventType Event, void *data)
{
	switch (Event)
	{
	case EventType::FRONT_END_INITIALIZED:
	{
		InitUIHooks();
		break;
	}
	case EventType::SWITCH_CAMERA:
	{
		auto activeCamera = Manager::GetScene()->GetActiveCamera();
		activeCamera->SetDebugView(true);
		activeCamera->renderer->SetIsRendered(true);

		activeSceneCamera++;
		if (activeSceneCamera == sceneCameras.size()) {
			activeSceneCamera = 0;
		}

		Manager::GetScene()->SetActiveCamera(sceneCameras[activeSceneCamera]);
		cameraInput->camera = Manager::GetScene()->GetActiveCamera();
		break;
	}
	default:
		break;
	}
}

void Game::OnEvent(const string& eventID, void *data)
{
}

void Game::InitSceneCameras()
{
	activeSceneCamera = 0;
	sceneCameras.push_back(gameCamera);
	sceneCameras.push_back(freeCamera);
	sceneCameras.push_back(Spot);
	sceneCameras.push_back(Sun);

	Manager::GetScene()->GetActiveCamera()->SetDebugView(false);
}

void Game::ToggleRenderingToOculusRift()
{
#ifdef OCULUS_RIFT_HMD
	oculusHMD->SetRenderToHMD(!oculusHMD->IsRenderingToHMD());
	if (oculusHMD->IsRenderingToHMD()) {
		window->SetVSync(false); 
	}
	else {
		window->SetVSync(true);
	}
#endif
}
