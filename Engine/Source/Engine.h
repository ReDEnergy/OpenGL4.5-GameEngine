#pragma once

// Templates

#include <templates/singleton.h>

// Includes
#ifdef GL_ENABLED
#include <include/gl.h>
#endif

#include <include/glm.h>
#include <include/glm_utils.h>
#include <include/math.h>
#include <include/utils.h>

#ifdef PHYSICS_ENGINE
#include <include/havok.h>
#endif

// Audio
#ifdef USE_AUDIO_MANAGER
#include <Audio/SoundFX.h>
#include <Audio/AudioStream.h>
#endif

// Core Modules
#include <Core/World.h>
#include <Core/Engine.h>
#include <Core/Camera/Camera.h>
#include <Core/WindowObject.h>
#include <Core/WindowManager.h>
#include <Core/Camera/ThirdPersonCamera.h>

#include <Core/Object.h>
#include <Core/GameObject.h>
#include <Core/InputSystem.h>

// Components
#include <Component/AABB.h>
#include <Component/Animation/AnimationController.h>
#include <Component/AudioSource.h>
#include <Component/Transform/Transform.h>
#include <Component/Transform/FreezeTransform.h>
#include <Component/Transform/LimitedTransform.h>
#include <Component/Mesh.h>
#include <Component/MeshRenderer.h>
#include <Component/ObjectInput.h>
#include <Component/Particles/ParticleEffect.h>
#include <Component/Text.h>
#include <Component/SkinnedMesh.h>
#include <Component/Renderer.h>

#ifdef PHYSICS_ENGINE
#include <Component/Physics.h>
#endif

// Events
#include <Event/TimerEvent.h>
#include <Event/EventType.h>
#include <Event/SimpleTimer.h>

// SkeletalSystem
#include <SkeletalSystem/SkeletalJoint.h>

// Audio
#include <Audio/AudioStream.h>
#include <Audio/SoundFX.h>

// Motion
#include <Motion/MotionTransition.h>

// Managers
#include <Manager/Manager.h>
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

// Debugging
#include <Debugging/TextureDebugger.h>
#include <Debugging/ProfileTimer.h>

#ifdef PHYSICS_ENGINE
#include <Manager/HavokCore.h>
#include <Manager/PhysicsManager.h>
#endif

// UI Managers
#include <UI/ColorPicking/ColorPicking.h>
#include <UI/DebugOverlayText.h>
#include <UI/MenuSystem.h>

// Lighting
#include <Lighting/Light.h>
#include <Lighting/PointLight.h>
#include <Lighting/DirectionalLight.h>
#include <Lighting/SpotLight.h>

// Input
#include <InputComponent/CameraInput.h>
#include <InputComponent/CameraDebugInput.h>
#include <InputComponent/DebugInput.h>
#include <InputComponent/EditorInput.h>

// GPU
#include <GPU/FrameBuffer.h>
#include <GPU/Texture.h>
#include <GPU/Shader.h>

#ifdef GL_ENABLED
#include <GPU/SSBO.h>
#endif

// Rendering
#include <Rendering/SSAO.h>
#include <Rendering/DirectOpenGL.h>
#include <Rendering/ShadowMapping.h>

// Utils
#ifdef GL_ENABLED
#include <Utils/GPU.h>
#endif
#include <Utils/OpenGL.h>
#include <Utils/Serialization.h>

// Prototyping
#include <Prototyping/Prototype.h>

// Templates
#include <templates/Singleton.h>

#ifdef GL_ENABLED
	// WARNING! GL is exposed
#endif

