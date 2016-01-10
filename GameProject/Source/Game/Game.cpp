#include "pch.h"
#include "Game.h"

#include <Game/Input/AnimationInput.h>
#include <Game/Input/SimpleObjectControl.h>
#include <Game/Input/GameInput.h>

#include <templates/Singleton.h>

#ifdef NDEBUG
// RELEASE BUILD
#endif
#ifdef _DEBUG
// DEBUG BUILD
#endif

//#define KINECT_MODULE
#ifdef KINECT_MODULE
#include <Kinect/KinectSensor.h>
#include <Kinect/SkeletalTracking.h>
#include <Kinect/Depth/KinectPointCloud.h>
#include <Kinect/KinectStreaming.h>
#include <SkeletalSystem/SkeletalSystem.h>
#include <Prototyping/TestConeConstraint.h>

KinectSensor *kinectSensor;
SkeletalTracking *skeletalTracking;
SkeletalSystem *skeletalSystem;
TestConeConstraint *testCone;
KinectStreaming *kinectStreaming;
KinectPointCloud *kinectPointCloud;
#endif

#define COMPUTE_TESTS
#ifdef COMPUTE_TESTS
#endif

// Add support for PhysX Engine
#ifdef PHYSX_ENGINE
#include <Physics/PhysX.h>

PhysXManager *physXManager;
PhysXRagdoll *ragdoll;
#endif

// Prototyping
#define PROTOTYPING
#ifdef PROTOTYPING
#include <Prototyping/ToFSimulation.h>
ToFSimulation *ToF;

#endif


Game::Game() {
}

Game::~Game() {
}

void Game::Init() {
	
	// Game resolution
	glm::ivec2 resolution = Engine::Window->resolution;
	float aspectRation = float(resolution.x) / resolution.y;

	// Cameras
	gameCamera = new Camera();
	gameCamera->SetPerspective(50, aspectRation, 0.001f, 1500);
	gameCamera->SetPosition(glm::vec3(0, 5, 5));
	gameCamera->SplitFrustum(5);
	gameCamera->transform->SetMoveSpeed(5);

	freeCamera = new Camera();
	freeCamera->SetPerspective(40, aspectRation, 0.001f, 500);
	freeCamera->SetPosition(glm::vec3(0.0f, 10.0f, 10.0f));
	freeCamera->Update();

	Manager::GetScene()->SetActiveCamera(gameCamera);

	// Lights
	Sun = new DirectionalLight();
	Sun->SetPosition(glm::vec3(0.0f, 50.0f, 0.0f));
	Sun->RotateOX(-460);
	Sun->RotateOY(360);
	Sun->SetOrthgraphic(40, 40, 0.1f, 200);
	Sun->transform->SetWorldRotation(glm::vec3(30, 60, 0));
	Sun->SetCamera(gameCamera);
	Sun->Update();

	Spot = new SpotLight();
	Spot->SetPosition(glm::vec3(5, 3, 0));
	Spot->SetPerspective(90, 1, 0.1f, 50);
	Spot->ComputeFrustum();
	Spot->transform->SetWorldRotation(glm::vec3(90, 0, 0));
	Manager::GetDebug()->Add(Spot);

	ShadowMap = new Texture();
	ShadowMap->Create2DTextureFloat(NULL, resolution.x, resolution.y, 4, 32);

	// --- Create FBO for rendering to texture --- //
	FBO = new FrameBuffer();
	FBO->Generate(resolution.x, resolution.y, 5);

	FBO_Light = new FrameBuffer();
	FBO_Light->Generate(resolution.x, resolution.y, 1, false);

	// Rendering 
	ssao = new SSAO();
	ssao->Init(resolution.x, resolution.y);

	ScreenQuad = Manager::GetResource()->GetGameObject("render-quad");
	ScreenQuad->Update();

	// --- Debugging --- //
	auto *TDBG = Manager::GetTextureDebugger();
	TDBG->SetChannel(0, FBO);
	TDBG->PushToChannel(0, FBO_Light->GetTexture(0));
	TDBG->SetChannel(1, Manager::GetPicker()->FBO);
	TDBG->PushToChannel(1, Manager::GetPicker()->FBO_Gizmo->GetTexture(0));

	// Listens to Events and Input

	SubscribeToEvent("barrels");
	SubscribeToEvent("barrels-light");
	SubscribeToEvent(EventType::SWITCH_CAMERA);

	cameraInput = new CameraInput(Manager::GetScene()->GetActiveCamera());
	cameraDebugInput = new CameraDebugInput(gameCamera);
	ObjectInput *DI = new DebugInput();
	ObjectInput *EI = new EditorInput();
	ObjectInput *GI = new GameInput(this);
	//ObjectControl *control = new ObjectControl(PLSC->transform);
	InputRules::PushRule(InputRule::R_GAMEPLAY);


	 //GameObjects
	//for (uint i = 0; i < 300; i++) {
	//	GameObject *tree = Manager::GetResource()->GetGameObject("bamboo");
	//	tree->transform->SetWorldPosition(glm::vec3(rand() % 100 - 50, 0, rand() % 100 - 50));
	//	Manager::GetScene()->AddObject(tree);
	//}


	// GameObjects
	//for (int i = -5; i < 5; i++) {
	//	for (int j = -5; j < 5; j++) {
	//		GameObject *ground = Manager::GetResource()->GetGameObject("ground");
	//		ground->transform->SetWorldPosition(glm::vec3(i * 10 + 5, -0.1f, j * 10 + 5));
	//		Manager::GetScene()->AddObject(ground);
	//	}
	//}

	InitSceneCameras();

	#ifdef PHYSICS_ENGINE
	Manager::GetHavok()->StepSimulation(0.016f);
	#endif

	Manager::GetScene()->Update();

	auto character = Manager::GetScene()->GetGameObject("Archer2", 1);
	AnimationInput *aInput = new AnimationInput(character);
	character->input = aInput;

	// Kinect Modules
	#ifdef KINECT_MODULE

	kinectSensor = Singleton<KinectSensor>::Instance();
	skeletalTracking = new SkeletalTracking();
	skeletalTracking->Init(kinectSensor);
	skeletalSystem = new SkeletalSystem();
	
	kinectStreaming = Singleton<KinectStreaming>::Instance();
	kinectStreaming->Init(kinectSensor);

	kinectPointCloud = new KinectPointCloud();

	// testCone = new TestConeConstraint();

	#endif

	#ifdef PHYSX_ENGINE
	physXManager = Singleton<PhysXManager>::Instance();
	physXManager->Init();

	//ragdoll = new PhysXRagdoll(physXManager);
	//ragdoll->Load("Resources//Physx//Archer.xml");
	#endif

	// Combine Rotation Test

	GameObject *box0 = Manager::GetResource()->GetGameObject("box");
	GameObject *box1 = Manager::GetResource()->GetGameObject("box");
	GameObject *box2 = Manager::GetResource()->GetGameObject("box");

	box0->transform = new LimitedTransform();
	box1->transform = new LimitedTransform();
	box2->transform = new LimitedTransform();

	new SimpleObjectControl(box0);
	box0->transform->SetWorldPosition(glm::vec3(5, 1, 5));
	box1->AddChild(box2);
	box0->AddChild(box1);

	box1->transform->SetLocalPosition(glm::vec3(3, 3, 3));
	box2->transform->SetLocalPosition(glm::vec3(3, -1, -2));

	Manager::GetScene()->AddObject(box0);


	// Joint Test

	//GameObject *box3 = Manager::GetResource()->GetGameObject("box");
	//GameObject *box4 = Manager::GetResource()->GetGameObject("box");

	//box3->transform = new LimitedTransform();
	//box4->transform = new LimitedTransform();

	//box3->transform->SetWorldPosition(glm::vec3(10, 10, 0));
	//box3->AddChild(box4);
	//box4->transform->SetLocalPosition(glm::vec3(7.5, 0, 0));

	//Manager::GetScene()->AddObject(box3);

	// Prototyping
	ToF = new ToFSimulation();
	ToF->Init(*FBO);

	wglSwapIntervalEXT(1);
};

void Game::FrameStart()
{
	Engine::Window->SetContext();
	Manager::GetEvent()->EmitSync(EventType::FRAME_START);
}

void Game::Update(float elapsedTime, float deltaTime) {

	Camera *activeCamera = Manager::GetScene()->GetActiveCamera();

	// -----------------------//
	// --- Sensors Update --- //
	// -----------------------//

	#ifdef KINECT_MODULE
	kinectSensor->Update(elapsedTime, deltaTime);
	skeletalTracking->Update();
	skeletalSystem->Update(skeletalTracking);
	kinectStreaming->Update();
	kinectPointCloud->Update();
	#endif

	// ---------------------------//
	// --- Physics Simulation --- //
	// ---------------------------//

	#ifdef PHYSICS_ENGINE
	Manager::GetHavok()->StepSimulation(deltaTime);
	physXManager->StepPhysics(deltaTime);
	//ragdoll->Update(deltaTime, activeCamera);
	#endif

	///////////////////////////////////////////////////////////////////////////
	// Update Scene

  	InputSystem::UpdateObservers(deltaTime);

	Manager::GetAudio()->Update(activeCamera);
	Manager::GetEvent()->Update();
	Manager::GetScene()->Update();
	Manager::GetScene()->LightSpaceCulling(gameCamera, Sun);
	Manager::GetPicker()->Update(activeCamera);
	Manager::GetPicker()->DrawSceneForPicking();
	Manager::GetDebug()->Update(activeCamera);
	Manager::GetEvent()->EmitSync(EventType::FRAME_UPDATE);

	#ifdef KINECT_MODULE
	// testCone->Update(deltaTime);
	#endif

	///////////////////////////////////////////////////////////////////////////
	// Scene Rendering

	if (Manager::GetRenderSys()->Is(RenderState::FORWARD)) {
		FrameBuffer::Unbind();
		FrameBuffer::Clear();
	}
	else {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		FBO->Bind(true);
	}

	Manager::GetScene()->Render(activeCamera);

	///////////////////////////////////////////////////////////////////////////
	// Shadows Casting
	{
		Sun->CastShadows();
		Spot->CastShadows();
		Sun->BakeShadows(FBO);
		Spot->BakeShadows(FBO);
	}

	#ifdef KINECT_MODULE
	skeletalTracking->Render(activeCamera);
	skeletalSystem->Render(activeCamera);
	kinectPointCloud->Render(activeCamera);
	#endif

	// ---------------------------//
	// --- Deferred Rendering --- //
	// ---------------------------//

	if (!Manager::GetRenderSys()->Is(RenderState::FORWARD)) {

		// --- Deferred Lighting --- //
		{
			FBO_Light->Bind();
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);
			Shader *DF = Manager::GetShader()->GetShader("deferred");
			DF->Use();

			FBO_Light->SendResolution(DF);
			activeCamera->BindPosition(DF->loc_eye_pos);
			activeCamera->BindViewMatrix(DF->loc_view_matrix);
			activeCamera->BindProjectionMatrix(DF->loc_projection_matrix);

			FBO->BindTexture(1, GL_TEXTURE0);
			FBO->BindTexture(2, GL_TEXTURE1);

			for (auto *light: Manager::GetScene()->GetPointLights()) {
				auto FACE = activeCamera->DistTo(light) < light->effectRadius ? OpenGL::CULL::FRONT : OpenGL::CULL::BACK;
				light->renderer->SetCulling(FACE);
				light->RenderDeferred(DF);
			}

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}

		// --- Screen Space Ambient Occlusion (SSAO) --- //
		if (Manager::GetRenderSys()->Is(RenderState::SS_AO)) {
			ssao->Update(FBO, activeCamera);
		}

		// --- Render to the screen --- //
		// ---   Composition step   --- //

		FrameBuffer::Unbind();
		FrameBuffer::Clear();
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		{
			Shader *Composition = Manager::GetShader()->GetShader("composition");
			Composition->Use();
			glUniform2f(Composition->loc_resolution, (float)Engine::Window->resolution.x, (float)Engine::Window->resolution.y);
			glUniform1i(Composition->loc_debug_view, Manager::GetDebug()->GetActiveState());
			glUniform1i(Composition->active_ssao, Manager::GetRenderSys()->Is(RenderState::SS_AO));
			glUniform1i(Composition->active_selection, Manager::GetPicker()->HasActiveSelection());
			activeCamera->BindProjectionDistances(Composition);

			FBO->BindTexture(0, GL_TEXTURE0);
			FBO_Light->BindTexture(0, GL_TEXTURE1);
			ShadowMap->BindToTextureUnit(GL_TEXTURE2);
			ssao->BindTexture(GL_TEXTURE3);
			FBO->BindDepthTexture(GL_TEXTURE4);
			Manager::GetDebug()->FBO->BindTexture(0, GL_TEXTURE5);
			Manager::GetDebug()->FBO->BindDepthTexture(GL_TEXTURE6);
			Manager::GetPicker()->FBO_Gizmo->BindTexture(0, GL_TEXTURE7);
				
			#ifdef KINECT_MODULE
			skeletalTracking->BindForComposition(GL_TEXTURE8, GL_TEXTURE9);
			skeletalSystem->BindForComposition(GL_TEXTURE10, GL_TEXTURE11);
			#endif

			ScreenQuad->Render(Composition);
		}
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}

	Manager::GetDebugText()->Update();
	Manager::GetScene()->FrameEnded();
	Manager::GetEvent()->EmitSync(EventType::FRAME_AFTER_RENDERING);
}

void Game::FrameEnd()
{
	Manager::GetTextureDebugger()->Render();
	Manager::GetMenu()->RenderMenu();
	Manager::GetScene()->FrameEnded();
	Manager::GetEvent()->EmitSync(EventType::FRAME_END);
}

void Game::BarrelPhysicsTest(bool pointLights)
{
#ifdef PHYSICS_ENGINE
	glm::vec3 pos = gameCamera->transform->GetWorldPosition();
	GameObject *barrel = Manager::GetResource()->GetGameObject("oildrum");
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

void Game::OnEvent(EventType Event, void *data) {
	switch (Event)
	{
	case EventType::SWITCH_CAMERA:

		Manager::GetScene()->GetActiveCamera()->SetDebugView(true);

		activeSceneCamera++;
		if (activeSceneCamera == sceneCameras.size()) {
			activeSceneCamera = 0;
		}

		Manager::GetScene()->SetActiveCamera(sceneCameras[activeSceneCamera]);
		Manager::GetScene()->GetActiveCamera()->SetDebugView(false);
		cameraInput->camera = Manager::GetScene()->GetActiveCamera();

	default:
		break;
	}
}

void Game::OnEvent(const string& eventID, void *data)
{
	if (eventID.compare("barrels-light") == 0) {
		BarrelPhysicsTest(true);
	}

	if (eventID.compare("barrels") == 0) {
		BarrelPhysicsTest(false);
	}
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
