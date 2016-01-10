#include "pch.h"
#include "Game.h"

#include <Game/Input/AnimationInput.h>
#include <Game/Input/SimpleObjectControl.h>
#include <Game/Input/GameInput.h>

#ifdef NDEBUG
// RELEASE BUILD
#endif
#ifdef _DEBUG
// DEBUG BUILD
#endif

#define KINECT_MODULE
#ifdef KINECT_MODULE
#include <Kinect/KinectSensor.h>
#include <Kinect/SkeletalTracking.h>
#include <SkeletalSystem/SkeletalSystem.h>

KinectSensor *kinect;
SkeletalTracking *skeletalTracking;
SkeletalSystem *skeletalSystem;
#endif

// Add support for PhysX Engine
#ifdef PHYSX_ENGINE
#include <Physics/PhysX.h>

PhysXManager *physXManager;
PhysXRagdoll *ragdoll;
#endif

Game::Game() {
}

Game::~Game() {
}

void Game::Init() {
	
	// Game resolution
	glm::ivec2 resolution = Manager::GetConfig()->resolution;
	float aspectRation = float(resolution.x) / resolution.y;

	// Cameras
	gameCamera = new Camera();
	gameCamera->SetPerspective(40, aspectRation, 0.1f, 1500);
	gameCamera->SetPosition(glm::vec3(0, 5, 5));
	gameCamera->SplitFrustum(5);
	gameCamera->transform->SetMoveSpeed(5);
	//gameCamera->Update();

	freeCamera = new Camera();
	freeCamera->SetPerspective(40, aspectRation, 0.1f, 500);
	freeCamera->SetPosition(glm::vec3(0.0f, 10.0f, 10.0f));
	freeCamera->Update();

	Manager::GetScene()->SetActiveCamera(gameCamera);

	// Lights
	Sun = new DirectionalLight();
	Sun->SetPosition(glm::vec3(0.0f, 50.0f, 0.0f));
	Sun->RotateOX(-460);
	Sun->RotateOY(360);
	Sun->SetOrthgraphic(40, 40, 0.1f, 200);
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
	FBO_Light->Generate(resolution.x, resolution.y, 1);

	// Rendering 
	ssao = new SSAO();
	ssao->Init(resolution.x, resolution.y);

	ScreenQuad = Manager::GetResource()->GetGameObject("render-quad");
	ScreenQuad->Update();

	// --- Debugging --- //
	DebugPanel = Manager::GetResource()->GetGameObject("render-quad");
	DebugPanel->UseShader(Manager::GetShader()->GetShader("debug"));
	DebugPanel->transform->SetScale(glm::vec3(0.5));
	DebugPanel->transform->SetWorldPosition(glm::vec3(0.5));

	// Listens to Events and Input

	SubscribeToEvent("barrels");
	SubscribeToEvent("barrels-light");
	SubscribeToEvent(EventType::SWITCH_CAMERA);
	SubscribeToEvent(EventType::CLOSE_MENU);
	SubscribeToEvent(EventType::OPEN_GAME_MENU);

	cameraInput = new CameraInput(Manager::GetScene()->GetActiveCamera());
	cameraDebugInput = new CameraDebugInput(gameCamera);
	ObjectInput *DI = new DebugInput();
	ObjectInput *EI = new EditorInput();
	ObjectInput *GI = new GameInput(this);
	//ObjectControl *control = new ObjectControl(PLSC->transform);
	InputRules::PushRule(InputRule::R_GAMEPLAY);


	// GameObjects
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
	/*
	for (auto *obj : Manager::GetScene()->activeObjects) {
		if (obj->audioSource)
			obj->audioSource->Play();
	}
	*/

	auto character = Manager::GetScene()->GetGameObject("Archer2", 1);
	AnimationInput *aInput = new AnimationInput(character);
	character->input = aInput;

	// Kinect Module
	#ifdef KINECT_MODULE
	kinect = new KinectSensor();
	kinect->Init();
	skeletalTracking = new SkeletalTracking();
	skeletalTracking->Init(kinect);
	skeletalSystem = new SkeletalSystem();
	
	#endif

	#ifdef PHYSX_ENGINE
	physXManager = new PhysXManager();
	physXManager->Init();

	ragdoll = new PhysXRagdoll(physXManager);
	ragdoll->Load("Resources//Physx//Archer.xml");
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

	GameObject *box3 = Manager::GetResource()->GetGameObject("box");
	GameObject *box4 = Manager::GetResource()->GetGameObject("box");

	box3->transform = new LimitedTransform();
	box4->transform = new LimitedTransform();

	box3->transform->SetWorldPosition(glm::vec3(10, 10, 0));
	box3->AddChild(box4);
	box4->transform->SetLocalPosition(glm::vec3(7.5, 0, 0));

	Manager::GetScene()->AddObject(box3);

	wglSwapIntervalEXT(1);
};

void Game::Update(float elapsedTime, float deltaTime) {

	Camera *activeCamera = Manager::GetScene()->GetActiveCamera();

	// -----------------------//
	// --- Sensors Update --- //
	// -----------------------//

	#ifdef KINECT_MODULE
	kinect->Update(elapsedTime, deltaTime);
	skeletalTracking->Update();
	skeletalSystem->Update(skeletalTracking);
	#endif

	// ---------------------------//
	// --- Physics Simulation --- //
	// ---------------------------//

	#ifdef PHYSICS_ENGINE
	Manager::GetHavok()->StepSimulation(deltaTime);
	physXManager->StepPhysics(deltaTime);
	ragdoll->Update(deltaTime, activeCamera);
	#endif

	// ---------------------//
	// --- Update Scene --- //
	// ---------------------//

  	InputSystem::UpdateObservers(deltaTime);

	Manager::GetAudio()->Update(activeCamera);
	Manager::GetEvent()->Update();
	Manager::GetScene()->Update();

	if (Manager::GetDebug()->debugView) {
		Manager::GetDebug()->BindForRendering(activeCamera);
		Manager::GetDebug()->Render(activeCamera);
	}

	Manager::GetPicker()->Update(activeCamera);
	Manager::GetScene()->LightSpaceCulling(gameCamera, Sun);

	// ------------------------//
	// --- Scene Rendering --- //
	// ------------------------//

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	if (Manager::GetRenderSys()->Is(RenderState::FORWARD)) {
		FrameBuffer::Unbind();
		FrameBuffer::Clear();
	}
	else {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		FBO->Bind(true);
	}

	{
		Shader *R2T = Manager::GetShader()->GetShader("rendertargets");
		R2T->Use();
		activeCamera->BindPosition(R2T->loc_eye_pos);
		activeCamera->BindViewMatrix(R2T->loc_view_matrix);
		activeCamera->BindProjectionMatrix(R2T->loc_projection_matrix);

		Shader *R2TSk = Manager::GetShader()->GetShader("r2tskinning");
		R2TSk->Use();
		activeCamera->BindPosition(R2TSk->loc_eye_pos);
		activeCamera->BindViewMatrix(R2TSk->loc_view_matrix);
		activeCamera->BindProjectionMatrix(R2TSk->loc_projection_matrix);

		// TODO - IVESTIGATE BUG - IF THERE ARE NOT SKINNED NOTHING WILL BE RENDERD
		for (auto *obj : Manager::GetScene()->frustumObjects) {
			if (obj->mesh && obj->mesh->meshType == MeshType::SKINNED) {
				obj->mesh->Update();
				Manager::GetShader()->PushState(R2TSk);
				obj->Render(R2TSk);
			}
			else {
				Manager::GetShader()->PushState(R2T);
				obj->Render(R2T);
			}
		}
	}

	// -------------------------------------------//
	// --- Camera Culling and Shadows Casting --- //
	// -------------------------------------------//
	{
		Sun->CastShadows(gameCamera);
		Spot->CastShadows();
	}


	///////////////////////////////////////////////////////////////////////////
	// Accumulate shadows using a compute shader
	{
		Shader *sha = Manager::GetShader()->GetShader("CSMShadowMap");
		sha->Use();

		FBO->BindDepthTexture(GL_TEXTURE0);
		gameCamera->BindProjectionDistances(sha);

		glBindImageTexture(0, FBO->textures[0].GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(1, FBO->textures[1].GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(2, ShadowMap->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		{
			Sun->BindForUse(sha, gameCamera);
			glUniform1i(sha->loc_shadowID, 0);

			glDispatchCompute(GLuint(UPPER_BOUND(FBO->GetResolution().x, 16)), GLuint(UPPER_BOUND(FBO->GetResolution().y, 16)), 1);
			glFinish();
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
		}

		// Take care - using texture binding from above
		sha = Manager::GetShader()->GetShader("ShadowMap");
		sha->Use();
		{
			Spot->BindForUse(sha);
			glUniform1i(sha->loc_shadowID, 100);

			glDispatchCompute(GLuint(UPPER_BOUND(FBO->GetResolution().x, 16)), GLuint(UPPER_BOUND(FBO->GetResolution().y, 16)), 1);
			glFinish();
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
		}
	}
	///////////////////////////////////////////////////////////////////////////	

	#ifdef KINECT_MODULE
	skeletalTracking->Render(activeCamera);
	skeletalSystem->Render(activeCamera);
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
			glEnable(GL_CULL_FACE);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);
			glCullFace(GL_BACK);

			Shader *DF = Manager::GetShader()->GetShader("deferred");
			DF->Use();

			FBO_Light->SendResolution(DF);
			activeCamera->BindPosition(DF->loc_eye_pos);
			activeCamera->BindViewMatrix(DF->loc_view_matrix);
			activeCamera->BindProjectionMatrix(DF->loc_projection_matrix);

			FBO->BindTexture(1, GL_TEXTURE0);
			FBO->BindTexture(2, GL_TEXTURE1);

			for (auto *light: Manager::GetScene()->lights) {
				(activeCamera->DistTo(light) < light->effectRadius) ? glCullFace(GL_FRONT) : glCullFace(GL_BACK);
				light->RenderDeferred(DF);
			}

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
			glDisable(GL_CULL_FACE);
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
			glUniform1i(Composition->active_ssao, Manager::GetRenderSys()->Is(RenderState::SS_AO));
			glUniform1i(Composition->loc_debug_view, Manager::GetDebug()->debugView);
			activeCamera->BindProjectionDistances(Composition);

			FBO->BindTexture(0, GL_TEXTURE0);
			FBO_Light->BindTexture(0, GL_TEXTURE1);
			ShadowMap->Bind(GL_TEXTURE2);
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

		// --- Debug View --- //
		if (Manager::GetRenderSys()->Is(RenderState::DEBUG)) {
			Shader *Debug = Manager::GetShader()->GetShader("debug");
			Debug->Use();
			glUniform1i(Debug->loc_debug_id, Manager::GetRenderSys()->debugParam);
			activeCamera->BindProjectionDistances(Debug);

			FBO->BindAllTextures();
			ssao->BindTexture(GL_TEXTURE4);
			FBO_Light->BindTexture(0, GL_TEXTURE5);
			FBO->BindDepthTexture(GL_TEXTURE6);
			Manager::GetPicker()->FBO_Gizmo->BindTexture(0, GL_TEXTURE7);
			Manager::GetPicker()->FBO->BindTexture(0, GL_TEXTURE8);
			Manager::GetDebug()->FBO->BindTexture(0, GL_TEXTURE9);
			Manager::GetDebug()->FBO->BindDepthTexture(GL_TEXTURE10);
			//Sun->FBO->BindTexture(0, GL_TEXTURE10);
			//Sun->FBO->BindDepthTexture(GL_TEXTURE11);
			//Spot->FBO->BindTexture(0, GL_TEXTURE12);
			//Spot->FBO->BindDepthTexture(GL_TEXTURE13);
			DebugPanel->Render();
		}

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}

	Manager::GetDebugText()->Update();
	Manager::GetMenu()->RenderMenu();
	Manager::GetScene()->FrameEnded();
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
			Manager::GetScene()->lights.push_back(obj);
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

void Game::OnEvent(EventType Event, Object *data) {
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

void Game::OnEvent(const char* eventID, Object *data)
{
	if (strcmp(eventID, "barrels-light") == 0) {
		BarrelPhysicsTest(true);
	}

	if (strcmp(eventID, "barrels") == 0) {
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
