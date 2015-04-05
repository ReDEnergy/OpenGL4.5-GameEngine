#include "Game.h"

#include <include/gl.h>
#include <include/glm.h>
#include <include/glm_utils.h>
#include <include/math.h>
#include <include/util.h>

#include <InputComponent/CameraInput.h>
#include <InputComponent/CameraDebugInput.h>
#include <InputComponent/DebugInput.h>
#include <Input/ObjectControl.h>

#include <Component/AABB.h>
#include <Component/Mesh.h>
#include <Component/Text.h>
#include <Component/Transform.h>
#include <Component/ObjectInput.h>
#include <Component/Renderer.h>

#include <Core/Camera/Camera.h>
#include <Core/Engine.h>
#include <Core/GameObject.h>
#include <Core/InputSystem.h>

#include <Game/Actors/Player.h>
//#include <Game/Actors/TransitionGate.h>
#include <Game/State/GameState.h>
#include <Game/Input/GameInput.h>

#include <GPU/FrameBuffer.h>
#include <GPU/Texture.h>
#include <GPU/Shader.h>

#include <Lighting/Light.h>
#include <Lighting/PointLight.h>
#include <Lighting/DirectionalLight.h>
#include <Lighting/SpotLight.h>

#include <Manager/AudioManager.h>
#include <Manager/EventSystem.h>
#include <Manager/DebugInfo.h>
#include <Manager/ConfigFile.h>
#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/RenderingSystem.h>
#include <UI/MenuSystem.h>

#include "State/GameState.h"

#ifdef PHYSICS_ENGINE
#include <Manager/HavokCore.h>
#include <Manager/PhysicsManager.h>
#endif


#include <Rendering/SSAO.h>

#include <UI/Overlay.h>

#include <templates/singleton.h>


// Havok
// #include <Common/Base/keycode.cxx>
// #include <Common/Base/Config/hkProductFeatures.cxx>

Texture *ShadowMap;
//PointLight *PLSC;

Game::Game() {
}

Game::~Game() {
}

void Game::Init() {

	SubscribeToEvent(EventType::DEBUG_BARREL_SPAWN);
	SubscribeToEvent(EventType::SWITCH_CAMERA);
	SubscribeToEvent(EventType::CLOSE_MENU);

	// Game resolution
	glm::ivec2 resolution = Manager::GetConfig()->resolution;
	float aspectRation = float(resolution.x) / resolution.y;

	// Cameras
	gameCamera = new Camera();
	gameCamera->SetPerspective(40, aspectRation, 0.1f, 150);
	gameCamera->SetPosition(glm::vec3(0, 5, 5));
	gameCamera->SplitFrustum(5);
	//gameCamera->Update();

	freeCamera = new Camera();
	freeCamera->SetPerspective(40, aspectRation, 0.1f, 500);
	freeCamera->SetPosition(glm::vec3(0.0f, 10.0f, 10.0f));
	//freeCamera->Update();

	activeCamera = gameCamera;
	cameraInput = new CameraInput(activeCamera);
	cameraDebugInput = new CameraDebugInput(gameCamera);

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
	Spot->SetDirection(glm::vec3(1, 0, 0));
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
	DebugPanel->transform->scale = glm::vec3(0.5);
	DebugPanel->transform->SetPosition(glm::vec3(0.5));

	ObjectInput *DI = new DebugInput();
	ObjectInput *GI = new GameInput(this);


#ifdef PHYSICS_ENGINE
	Manager::GetHavok()->StepSimulation(0.016f);
#endif

	// GameObjects
	player = new Player(*Manager::GetResource()->GetGameObject("player"));
	for (int i = 0; i < 7; i++) {
		for (int j = -3; j < 3; j++) {
			GameObject *ground = Manager::GetResource()->GetGameObject("ground");
			ground->transform->SetPosition(glm::vec3(i * 10, 0, j * 10));
			ground->SetupAABB();
			Manager::GetScene()->AddObject(ground);
		}
	}

	//TransitionGate *gate = new TransitionGate(*Manager::GetResource()->GetGameObject("gate"));
	//Manager::GetScene()->AddObject(gate);

	overlay = new Overlay();

	for (PointLight *light: Manager::GetScene()->lights) {
		light->SetDebugView(true);
	}

	//ObjectControl *control = new ObjectControl(PLSC->transform);

};


void Game::Update(float elapsedTime, float deltaTime) {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	double startT = glfwGetTime();

	if (RuntimeState::STATE == RunState::GAMEPLAY) {

		// ---------------------------//
		// --- Physics Simulation --- //
		// ---------------------------//

#ifdef PHYSICS_ENGINE
		Manager::GetHavok()->StepSimulation(deltaTime);
#endif
		// -----------------------//
		// --- Update Objects --- //
		// -----------------------//
  		InputSystem::UpdateObservers(deltaTime);

		Manager::GetScene()->Update();

		for (auto *obj: Manager::GetScene()->activeObjects) {
			obj->Update();
		}

		player->Update(deltaTime);
	}

	if (Manager::GetDebug()->debugView) {
		Manager::GetDebug()->BindForRendering(activeCamera);
		Manager::GetDebug()->Render(activeCamera);
	}

	// ------------------------//
	// --- Scene Rendering --- //
	// ------------------------//

	if (Manager::GetRenderSys()->Is(RenderState::FORWARD)) {
		FrameBuffer::Unbind();
		FrameBuffer::Clear();
	}
	else {
		FBO->Bind();
	}

	{
		Shader *R2T = Manager::GetShader()->GetShader("rendertargets");
		R2T->Use();

		activeCamera->BindPosition(R2T->loc_eye_pos);
		activeCamera->BindViewMatrix(R2T->loc_view_matrix);
		activeCamera->BindProjectionMatrix(R2T->loc_projection_matrix);

		for (auto *obj : Manager::GetScene()->frustumObjects) {
			obj->Render(R2T);
		}
		player->Render(R2T);
	}

	// ---------------------//
	// --- Cast Shadows --- //
	// ---------------------//
	{
		gameCamera->UpdateBoundingBox(Sun);
		for (auto *obj : Manager::GetScene()->activeObjects) {
			if (obj->aabb) {
				obj->aabb->Update(Sun->transform->rotationQ);
			}
		}
		Manager::GetScene()->FrustumCulling(gameCamera);
		Sun->CastShadows(gameCamera);
	}
	{
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
				(activeCamera->DistTo(light) < light->transform->scale.x) ? glCullFace(GL_FRONT) : glCullFace(GL_BACK);
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

			ScreenQuad->Render(Composition);
		}

		// --- Debug View --- //
		if (Manager::GetRenderSys()->Is(RenderState::DEBUG)) {
			Shader *Debug = Manager::GetShader()->GetShader("debug");
			Debug->Use();
			glUniform1i(Debug->loc_debug_id, Manager::GetRenderSys()->debugParam);
			activeCamera->BindProjectionDistances(Debug);
			//glm::BindUniform3f(Debug->loc_eye_pos, PLSC->transform->position);

			glDisable(GL_DEPTH_TEST);
			FBO->BindAllTextures();
			ssao->BindTexture(GL_TEXTURE4);
			FBO_Light->BindTexture(0, GL_TEXTURE5);
			FBO->BindDepthTexture(GL_TEXTURE6);
			ShadowMap->Bind(GL_TEXTURE8);
			Sun->FBO->BindTexture(0, GL_TEXTURE10);
			Sun->FBO->BindDepthTexture(GL_TEXTURE11);
			Spot->FBO->BindTexture(0, GL_TEXTURE12);
			Spot->FBO->BindDepthTexture(GL_TEXTURE13);
			//PLSC->BindTexture(GL_TEXTURE14);

			DebugPanel->Render();

			glEnable(GL_DEPTH_TEST);
		}

		glDepthMask(GL_TRUE);
	}



	if (RuntimeState::STATE == RunState::PAUSE_MENU) {
		Manager::GetMenu()->RenderMenu();
	}
	else {
		overlay->Update(deltaTime);
	}
	
	double endT = glfwGetTime();
	//fprintf(stderr, "FRAME TIME: %.2lf ms, DELTA TIME: %d ms\n", (endT - startT) * 1000, int(deltaTime * 1000));
}

void Game::BarrelPhysicsTest() {
#ifdef PHYSICS_ENGINE
	glm::vec3 pos = player->transform->position;
	GameObject *barrel = Manager::GetResource()->GetGameObject("oildrum");
	for (int i=0; i<100; i++) {
		GameObject *box = new GameObject(*barrel);
		box->transform->position = pos + glm::vec3(rand() % 10 - 5, rand() % 5 + 5, rand() % 10 - 5);
		Manager::GetScene()->AddObject(box);
	}
#endif
};

void Game::OnEvent(EventType Event, Object *data) {
	switch (Event)
	{
		case EventType::DEBUG_BARREL_SPAWN:
			BarrelPhysicsTest();
			return;
		case EventType::SWITCH_CAMERA:
			// Free roam camera
			if (activeCamera == gameCamera) {
				gameCamera->SetDebugView(true);
				Sun->SetDebugView(true);
				activeCamera = freeCamera;
				InputRules::SetRule(InputRule::R_EDITOR);
			}
			// Sun camera
			else if (activeCamera == freeCamera) {
				activeCamera = Spot;
				//Spot->SetDebugView(false);
			}
			// Game camera
			else {
				activeCamera = gameCamera;
				gameCamera->SetDebugView(false);
				InputRules::SetRule(InputRule::R_GAMEPLAY);
			}
			cameraInput->camera = activeCamera;
		case EventType::CLOSE_MENU:
			RuntimeState::STATE = RunState::GAMEPLAY;
		default:
			break;
	}
}