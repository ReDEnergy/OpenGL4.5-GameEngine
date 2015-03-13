#pragma once

#include <include/dll_export.h>
#include <include/pugixml.h>

class AudioManager;
class ColorPicking;
class DebugInfo;
class EventSystem;
class FontManager;
class MenuSystem;
class ResourceManager;
class TextureManager;
class SceneManager;
class ShaderManager;
class ConfigFile;
class RenderingSystem;

#ifdef PHYSICS_ENGINE
class HavokCore;
class PhysicsManager;
#endif

using namespace std;

class Manager
{
	private:
		Manager() {};
		~Manager() {};

		static const char* GetEntry(pugi::xml_node &config, const char *fileType);

	public:
		DLLExport static void Init();
		DLLExport static void LoadConfig();
		DLLExport static DebugInfo* GetDebug();
		DLLExport static SceneManager* GetScene();
		DLLExport static ResourceManager* GetResource();
		DLLExport static ShaderManager* GetShader();
		DLLExport static RenderingSystem* GetRenderSys();
		DLLExport static EventSystem* GetEvent();
		DLLExport static MenuSystem* GetMenu();
		DLLExport static TextureManager* GetTexture();
		DLLExport static ConfigFile* GetConfig();

	public:
		static AudioManager *Audio;
		static ColorPicking *ColorPick;
		static DebugInfo *Debug;
		static EventSystem *Event;
		static FontManager *Font;
		static MenuSystem *Menu;
		static ResourceManager *Resource;
		static TextureManager *Texture;
		static SceneManager *Scene;
		static ShaderManager *Shader;
		static ConfigFile *Config;
		static RenderingSystem *RenderSys;
#ifdef PHYSICS_ENGINE
		static HavokCore *Havok;
		static PhysicsManager *Physics;
#endif

	private:
		static string configPath;
};