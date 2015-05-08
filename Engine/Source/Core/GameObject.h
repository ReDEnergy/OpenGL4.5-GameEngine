#pragma once

#include <include/glm.h>
#include <include/dll_export.h>

#include <Core/Object.h>

class AABB;
class AudioSource;
class Mesh;
class Shader;
class Physics;
class Renderer;
class Transform;
class ObjectInput;

using namespace std;

class DLLExport GameObject: virtual public Object
{
	friend class SceneManager;

	public:
		GameObject(const char *name);
		GameObject(const GameObject &obj);
		virtual ~GameObject();

		void Init();
		bool ColidesWith(GameObject *object);
		float DistTo(GameObject *object);

		virtual void Render() const;
		virtual void Render(const Shader *shader) const;
		virtual void RenderInstanced(const Shader *shader, unsigned int instances) const;
		virtual void RenderDebug(const Shader *shader) const {};

		void SetupAABB();
		void SetDebugView(bool value);
		void SetAudioSource(AudioSource *source);

		virtual void Update();
		virtual void UseShader(Shader *shader);

	public:
		glm::vec3 colorID;

		AudioSource *audioSource = nullptr;
		AABB		*aabb = nullptr;
		Mesh		*mesh = nullptr;
		ObjectInput	*input = nullptr;
		Renderer	*renderer = nullptr;
		Transform	*transform = nullptr;
		Shader		*shader	= nullptr;
		#ifdef PHYSICS_ENGINE
		Physics *physics = nullptr;
		#endif

	protected:
		char *refID = nullptr;
		unsigned int instanceID = -1;
		bool debugView;
};

// TODO idea
// Create a temporary freeze state for changing various parameters and revert