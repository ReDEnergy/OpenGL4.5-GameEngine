#pragma once

class Engine;
class Camera;
class ThirdPersonCamera;

class Object;
class GameObject;
class InputSystem;

// Components
class AABB;
class AudioSource;
class LimitedTransform;
class Mesh;
class ObjectInput;
class Text;
class SkinnedMesh;
class Transform;
class Renderer;

// Debugging
class ProfileTimer;

// Audio
class AudioStream;
class SoundFX;

// Managers
class Manager;
class AudioManager;
class ColorManager;
class ConfigFile;
class DebugInfo;
class EventSystem;
class FontManager;
class RenderingSystem;
class ResourceManager;
class SceneManager;
class ShaderManager;
class TextureManager;

#ifdef PHYSICS_ENGINE
class HavokCore;
class PhysicsManager;
#endif

// UI Managers
class ColorPicking;
class DebugOverlayText;
class MenuSystem;

// Lighting
class Light;
class PointLight;
class DirectionalLight;
class SpotLight;

// Input
class CameraInput;
class CameraDebugInput;
class DebugInput;
class EditorInput;
class ObjectControl;

// GPU
class FrameBuffer;
class Texture;
class Shader;
class GPUBuffers;

// Rendering
template <class StorageEntry>
class SSBO;
class SSAO;

class ShadowMapping;

// Events
class EventListener;