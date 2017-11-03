#pragma once
#include <include/dll_export.h>

class AudioManager;
class ColorManager;
class ColorPicking;
class DebugInfo;
class DirectOpenGL;
class EventSystem;
class FontManager;
class MenuSystem;
class ResourceManager;
class TextureManager;
class SceneManager;
class ShaderManager;
class ConfigFile;
class RenderingSystem;
class DebugOverlayText;
class TextureDebugger;

#ifdef PHYSICS_ENGINE
class PhysXCore;
class PhysicsManager;
#endif

class Manager
{
	private:
		Manager() = delete;
		~Manager() = delete;

	public:
		DLLExport static void Init();
		DLLExport static void LoadConfig();

		DLLExport static AudioManager*		GetAudio();
		DLLExport static ColorPicking*		GetPicker();
		DLLExport static ColorManager*		GetColor();
		DLLExport static DebugInfo*			GetDebug();
		DLLExport static DirectOpenGL*		GetDirectGL();
		DLLExport static SceneManager*		GetScene();
		DLLExport static ResourceManager*	GetResource();
		DLLExport static ShaderManager*		GetShader();
		DLLExport static RenderingSystem*	GetRenderSys();
		DLLExport static EventSystem*		GetEvent();
		DLLExport static MenuSystem*		GetMenu();
		DLLExport static TextureManager*	GetTexture();
		DLLExport static TextureDebugger*	GetTextureDebugger();
		DLLExport static ConfigFile*		GetConfig();
		DLLExport static DebugOverlayText*	GetDebugText();

		#ifdef PHYSICS_ENGINE
		DLLExport static PhysicsManager*	GetPhysics();
		//DLLExport static PhysXCore*		GetPhysics();
		#endif

		#ifdef ENGINE_DLL_EXPORTS

	public:
		static AudioManager		*Audio;
		static ColorManager		*Color;
		static ColorPicking		*Picker;
		static DebugInfo		*Debug;
		static DirectOpenGL		*DirectGL;
		static EventSystem		*Event;
		static FontManager		*Font;
		static MenuSystem		*Menu;
		static ResourceManager	*Resource;
		static TextureManager	*Texture;
		static TextureDebugger	*TextureDBG;
		static SceneManager		*Scene;
		static ShaderManager	*Shader;
		static ConfigFile		*Config;
		static RenderingSystem	*RenderSys;
		static DebugOverlayText	*DebugText;

		#ifdef PHYSICS_ENGINE
		static PhysXCore		*PhysicsCore;
		static PhysicsManager	*PhysicsSystem;
		#endif

		#endif
};