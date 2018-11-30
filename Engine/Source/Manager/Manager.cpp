#include "Manager.h"

#include <iostream>

#include <include/gl.h>
#include <include/glm_utils.h>
#include <templates/singleton.h>

#include <Component/AABB.h>

#include <Core/Engine.h>
#include <Core/InputSystem.h>
#include <Core/WindowManager.h>
#include <Core/WindowObject.h>

#include <Manager/AudioManager.h>
#include <Manager/ColorManager.h>
#include <Manager/ConfigFile.h>
#include <Manager/DebugInfo.h>
#include <Manager/EventSystem.h>
#include <Manager/FontManager.h>
#include <Manager/RenderingSystem.h>
#include <Manager/ResourceManager.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/TextureManager.h>

#include <Component/Mesh.h>

#include <Debugging/TextureDebugger.h>

#include <Rendering/DirectOpenGL.h>

#ifdef PHYSICS_ENGINE
#include <Physics/PhysicsManager.h>
#include <Physics/PhysXCore.h>
#endif

#include <UI/DebugOverlayText.h>
#include <UI/MenuSystem.h>
#include <UI/ColorPicking/ColorPicking.h>

AudioManager*		Manager::Audio = nullptr;
DebugInfo*			Manager::Debug = nullptr;
ColorManager*		Manager::Color = nullptr;
ColorPicking*		Manager::Picker = nullptr;
ConfigFile*			Manager::Config = nullptr;
DebugOverlayText*	Manager::DebugText = nullptr;
DirectOpenGL*		Manager::DirectGL = nullptr;
EventSystem*		Manager::Event = nullptr;
FontManager*		Manager::Font = nullptr;
MenuSystem*			Manager::Menu = nullptr;
ResourceManager*	Manager::Resource = nullptr;
SceneManager*		Manager::Scene = nullptr;
ShaderManager*		Manager::Shader = nullptr;
TextureManager*		Manager::Texture = nullptr;
TextureDebugger*	Manager::TextureDBG = nullptr;
RenderingSystem*	Manager::RenderSys = nullptr;

#ifdef PHYSICS_ENGINE
PhysXCore*			Manager::PhysicsCore = nullptr;
PhysicsManager*		Manager::PhysicsSystem = nullptr;
#endif

void Manager::Init()
{
	// Init static objects
	InputSystem::Init();
	WindowManager::Init();

	// Load configuration
	Config = SINGLETON(ConfigFile);
	Config->Load("config.xml");

	// Create the default window
	auto window = new WindowObject(*(Config->windowProperties));

	// Init OpenGL
#ifndef OPENGL_ES
	glewExperimental = true;
#endif

	GLenum err = glewInit();
	if (GLEW_OK != err) 	// Serious problem
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(0);
	}

	CheckOpenGLError();

	// Init Managers
	Debug		= SINGLETON(DebugInfo);
	DebugText	= SINGLETON(DebugOverlayText);
	
	Audio		= SINGLETON(AudioManager);
	Event		= SINGLETON(EventSystem);
	Color		= SINGLETON(ColorManager);
	Picker		= SINGLETON(ColorPicking);
	Font		= SINGLETON(FontManager);
	TextureDBG	= SINGLETON(TextureDebugger);
	DirectGL	= SINGLETON(DirectOpenGL);
	Texture		= SINGLETON(TextureManager);
	RenderSys	= SINGLETON(RenderingSystem);

	RenderSys->Init();

	CheckOpenGLError();

	#ifdef PHYSICS_ENGINE
	PhysicsCore = SINGLETON(PhysXCore);
	PhysicsSystem = SINGLETON(PhysicsManager);
	#endif

	CheckOpenGLError();

	Resource	= SINGLETON(ResourceManager);
	Shader		= SINGLETON(ShaderManager);
	Scene		= SINGLETON(SceneManager);
	Menu		= SINGLETON(MenuSystem);

	CheckOpenGLError();

	Manager::LoadConfig();
}

// Load configuration file
void Manager::LoadConfig()
{
	auto window = WindowManager::GetDefaultWindow();
	window->MakeCurrentContext();
	window->SetVSync(RenderSys->Is(RenderState::VSYNC));
	CheckOpenGLError();

	////////////////////////////////////////

	Debug->Init();
	#ifdef PHYSICS_ENGINE
	PhysicsCore->Init();
	#endif


	Texture->Init();
	Audio->Init();
	Font->Init();

	Shader->SetConfigFiles(Config->GetResourceFiles("shaders"));
	Resource->Load(Config->GetResourceFileLoc("resource").c_str());
	Menu->Load(Config->GetResourceFileLoc("menu").c_str());
	Scene->LoadScene(Config->GetResourceFileLoc("scene").c_str());

	AABB::Init();
	Picker->Init();
	DirectGL->Init();
	TextureDBG->Init();
	Scene->Init();
}

DLLExport AudioManager* Manager::GetAudio()
{
	return Audio;
}

ColorPicking * Manager::GetPicker()
{
	return Picker;
}

DebugInfo* Manager::GetDebug()
{
	return Debug;
}

DLLExport DirectOpenGL* Manager::GetDirectGL()
{
	return DirectGL;
}

SceneManager* Manager::GetScene()
{
	return Scene;
}

ColorManager * Manager::GetColor()
{
	return Color;
}

ResourceManager* Manager::GetResource()
{
	return Resource;
}

ShaderManager* Manager::GetShader()
{
	return Shader;
}

RenderingSystem* Manager::GetRenderSys()
{
	return RenderSys;
}

EventSystem* Manager::GetEvent()
{
	return Event;
}

MenuSystem* Manager::GetMenu()
{
	return Menu;
}

TextureManager* Manager::GetTexture()
{
	return Texture;
}

TextureDebugger * Manager::GetTextureDebugger()
{
	return TextureDBG;
}

ConfigFile* Manager::GetConfig()
{
	return Config;
}

DebugOverlayText * Manager::GetDebugText()
{
	return DebugText;
}

#ifdef PHYSICS_ENGINE
PhysicsManager* Manager::GetPhysics()
{
	return PhysicsSystem;
}
#endif
