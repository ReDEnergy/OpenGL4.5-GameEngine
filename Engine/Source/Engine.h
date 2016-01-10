#pragma once

// Includes
#include <include/gl.h>
#include <include/gl_utils.h>
#include <include/glm.h>
#include <include/glm_utils.h>
#include <include/assimp.h>
#include <include/assimp_utils.h>
#include <include/math.h>
#include <include/utils.h>

#ifdef PHYSICS_ENGINE
#include <include/havok.h>
#endif

// Core Modules
#include <Core/World.h>
#include <Core/Engine.h>
#include <Core/Camera/Camera.h>
#include <Core/WindowObject.h>
#include <Core/Camera/ThirdPersonCamera.h>

#include <Core/Object.h>
#include <Core/GameObject.h>
#include <Core/InputSystem.h>

// Debugging
#include <Debugging/TextureDebugger.h>

// Components
#include <Component/AABB.h>
#include <Component/AudioSource.h>
#include <Component/Animation/AnimationController.h>
#include <Component/Transform/Transform.h>
#include <Component/Transform/LimitedTransform.h>
#include <Component/Mesh.h>
#include <Component/ObjectInput.h>
#include <Component/Text.h>
#include <Component/SkinnedMesh.h>
#include <Component/Renderer.h>

// SkeletalSystem
#include <SkeletalSystem/SkeletalJoint.h>

// Audio
#include <Audio/AudioStream.h>
#include <Audio/SoundFX.h>

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
#include <Input/ObjectControl.h>

// GPU
#include <GPU/FrameBuffer.h>
#include <GPU/Texture.h>
#include <GPU/Shader.h>

// Rendering
#include <Rendering/SSAO.h>
#include <Rendering/ShadowMapping.h>

// Utils
#include <Utils/OpenGL.h>

// Physics
#ifdef PHYSICS_ENGINE
#endif