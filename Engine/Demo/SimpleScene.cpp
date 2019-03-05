#include "SimpleScene.h"

#define GL_ENABLED
#include <Engine.h>

using namespace std;

SimpleScene::SimpleScene()
{
}

SimpleScene::~SimpleScene()
{
}

void SimpleScene::AddGameObject(GameObject *go)
{
	gameObjects.push_back(go);
}

void SimpleScene::RemoveGameObject(GameObject * go)
{
	cout << "[RemoveGameObject] must be implemented" << endl;
}

Shader* SimpleScene::GetStandardShader() const
{
	return sceneShader;
}

Shader * SimpleScene::GetScreenShader() const
{
	return screenShader;
}

void SimpleScene::InitResources(WindowObject *window)
{
	World::window = window;

	camera = new Camera();
	camera->SetPerspective(60, window->props.aspectRatio, 0.01f, 200);
	camera->transform->SetMoveSpeed(2);
	camera->transform->SetWorldPosition(glm::vec3(0, 1.6f, 2.5));
	camera->transform->SetWorldRotation(glm::vec3(-15, 0, 0));
	camera->Update();

	// Attach camera controller
	cameraInput = new CameraInput(camera);
	cameraInput->AttachTo(window);
	AttachTo(window);

	xozPlane = Manager::GetResource()->GetGameObject("grid");
	xozPlane->meshRenderer->InitForNewContext();
	xozPlane->renderer->SetCulling(OpenGL::CULL::NONE);

	// OpenGL resources
	screenQuad = Manager::GetResource()->GetGameObject("screen-quad");
	screenQuad->meshRenderer->InitForNewContext();
	screenQuad->renderer->SetCulling(OpenGL::CULL::NONE); 

	box = Manager::GetResource()->GetGameObject("box");
	box->meshRenderer->InitForNewContext();

	{
		vector<glm::vec3> positions = { glm::vec3(0, 0, 0), glm::vec3(0, 1, 0) };
		vector<glm::vec3> normals = { glm::vec3(0, 0, 0), glm::vec3(0, 1, 0) };
		vector<glm::vec2> tex_coord = { glm::vec2(0, 0), glm::vec2(0, 1) };
		vector<unsigned int> indices = { 0, 1 };

		Mesh *mesh = new Mesh();
		mesh->InitFromData(positions, normals, tex_coord, indices);

		coordinateSystem = new GameObject("simple-line");
		coordinateSystem->SetMesh(mesh);
		coordinateSystem->meshRenderer->SetGLDrawMode(GL_LINES);
		coordinateSystem->transform->SetWorldPosition(glm::vec3(0, 0.001f, 0));
		coordinateSystem->transform->SetScale(glm::vec3(1, 25, 1));
		coordinateSystem->renderer->SetCulling(OpenGL::CULL::NONE);
	}

	screenShader = Manager::GetShader()->GetShader("screen");
	sceneShader = Manager::GetShader()->GetShader("default");
	axisShader = Manager::GetShader()->GetShader("SimpleColor");

	locColor = axisShader->GetUniformLocation("color");
}

void SimpleScene::ReloadShaders()
{
	Manager::GetShader()->Reload("Engine");
	locColor = axisShader->GetUniformLocation("color");
}

void SimpleScene::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_F5) {
		ReloadShaders();
	}

	if (key == GLFW_KEY_ESCAPE) {
		World::window->Close();
	}
}

void SimpleScene::FrameStart()
{
	glm::ivec2 resolution = World::window->props.resolution;

	FrameBuffer::SetDefaultClearColor(glm::vec4(0.25f, 0.25f, 0.25f, 1));
	FrameBuffer::BindDefault(resolution, true);
}

void SimpleScene::Update(float deltaTime)
{
	RenderScene();
}

void SimpleScene::SetDrawGrid(bool value)
{
	xozPlane->renderer->SetIsRendered(value);
}

void SimpleScene::DrawCoordinatSystem() const
{
	bool lineSmoothing = false;
	float lineWidth = 1;
	//glGetFloatv(GL_LINE_WIDTH, &lineWidth);

	// Render the coordinate system
	axisShader->Use();
	camera->BindViewProj(axisShader);

	if (xozPlane->renderer->IsRendered())
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(1);

		glUniform3f(locColor, 0.5f, 0.5f, 0.5f);
		xozPlane->Render(axisShader);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glLineWidth(2);
	coordinateSystem->transform->SetWorldRotation(glm::quat());
	glUniform3f(locColor, 0, 1, 0);
	coordinateSystem->Render(axisShader);

	coordinateSystem->transform->SetWorldRotation(glm::vec3(0, 0, -90));
	glUniform3f(locColor, 1, 0, 0);
	coordinateSystem->Render(axisShader);

	coordinateSystem->transform->SetWorldRotation(glm::vec3(90, 0, 0));
	glUniform3f(locColor, 0, 0, 1);
	coordinateSystem->Render(axisShader);

	glLineWidth(lineWidth);
	//glDisable(GL_LINE_SMOOTH);

	CheckOpenGLError();
}

void SimpleScene::RenderScene() const
{
	auto PI = camera->GetProjectionInfo();
	PI.aspectRatio = World::window->props.aspectRatio;
	camera->SetProjection(PI);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	Manager::GetRenderSys()->DisableGlobalCulling();

	// Render visualization on texture
	if (sceneShader) {
		sceneShader->Use();

		// Bind MVP
		camera->BindViewProj(sceneShader);

		for (auto &obj : gameObjects) {
			obj->Render(sceneShader);
		}
	}

	DrawCoordinatSystem();
}
