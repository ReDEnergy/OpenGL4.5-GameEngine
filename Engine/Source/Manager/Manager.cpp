//#include <pch.h>
#include "Manager.h"

#include <iostream>

#include <include/glm_utils.h>
#include <templates/singleton.h>

#include <Component/AABB.h>

#include <Core/Engine.h>
#include <Core/InputSystem.h>
#include <Core/WindowManager.h>

#include <Input/InputRules.h>

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

#ifdef PHYSICS_ENGINE
#include <Manager/HavokCore.h>
#include <Manager/PhysicsManager.h>
#endif

#include <UI/MenuSystem.h>

AudioManager*		Manager::Audio = nullptr;
DebugInfo*			Manager::Debug = nullptr;
ColorManager*		Manager::Color = nullptr;
ConfigFile*			Manager::Config = nullptr;
EventSystem*		Manager::Event = nullptr;
FontManager*		Manager::Font = nullptr;
MenuSystem*			Manager::Menu = nullptr;
ResourceManager*	Manager::Resource = nullptr;
SceneManager*		Manager::Scene = nullptr;
ShaderManager*		Manager::Shader = nullptr;
TextureManager*		Manager::Texture = nullptr;
RenderingSystem*	Manager::RenderSys = nullptr;
#ifdef PHYSICS_ENGINE
HavokCore*			Manager::Havok = nullptr;
PhysicsManager*		Manager::Physics = nullptr;
#endif

void Manager::Init() {

	InputSystem::Init();

	Debug = Singleton<DebugInfo>::Instance();
	Debug->InitManager("Manager");

	InputRules::Init();
	RenderSys = Singleton<RenderingSystem>::Instance();
	Audio = Singleton<AudioManager>::Instance();
	Event = Singleton<EventSystem>::Instance();
	Color = Singleton<ColorManager>::Instance();
	Config = Singleton<ConfigFile>::Instance();
	Font = Singleton<FontManager>::Instance();

#ifdef PHYSICS_ENGINE
	Havok = Singleton<HavokCore>::Instance();
#endif

	Texture = Singleton<TextureManager>::Instance();
	Resource = Singleton<ResourceManager>::Instance();

#ifdef PHYSICS_ENGINE
	Physics = Singleton<PhysicsManager>::Instance();
#endif

	Scene = Singleton<SceneManager>::Instance();
	Shader = Singleton<ShaderManager>::Instance();
	Menu = Singleton<MenuSystem>::Instance();

	WindowManager::Init();

	Event->Clear();

	Manager::LoadConfig();
}

// Load configuration file
void Manager::LoadConfig() {

	Config->Load("config.xml");

	Engine::Window = WindowManager::NewWindow("Engine", Config->resolution, Config->position, true);

	RenderSys->Init();

	////////////////////////////////////////
	// TODO inspect if I can move these

	glewExperimental = true;
	glewInit();

	/* Force Vertical Sync */
	wglSwapIntervalEXT(1);

	////////////////////////////////////////

	Debug->Init();
#ifdef PHYSICS_ENGINE
	Havok->Init();
#endif
	Texture->Init();
	Audio->Init();
	Font->Init();
	Shader->Load(Config->GetResourceFileLoc("shaders"));
	Resource->Load(Config->GetResourceFileLoc("resource"));
	Menu->Load(Config->GetResourceFileLoc("menu"));
	Scene->LoadScene(Config->GetResourceFileLoc("scene"));

	AABB::Init();
}

DLLExport AudioManager* Manager::GetAudio()
{
	return Audio;
}

DebugInfo* Manager::GetDebug()
{
	return Debug;
}

SceneManager* Manager::GetScene()
{
	return Scene;
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

ConfigFile* Manager::GetConfig()
{
	return Config;
}

#ifdef PHYSICS_ENGINE
HavokCore* Manager::GetHavok()
{
	return Havok;
}

PhysicsManager* Manager::GetPhysics()
{
	return Physics;
}
#endif
