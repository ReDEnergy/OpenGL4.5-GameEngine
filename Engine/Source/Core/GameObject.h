#pragma once
#include <list>

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
	friend class AABB;

	public:
		GameObject(const char *referenceObject);
		GameObject(const GameObject &obj);
		virtual ~GameObject();

		void Init();
		void Clear();
		bool ColidesWith(GameObject *object);
		float DistTo(GameObject *object) const;

		virtual void Render() const;
		virtual void Render(const Shader *shader) const;
		virtual void RenderInstanced(const Shader *shader, unsigned int instances) const;
		virtual void RenderDebug(const Shader *shader) const;
		virtual void RenderForPicking(const Shader *shader) const;

		void SetMesh(Mesh *mesh);
		void SetupAABB();
		void SetDebugView(bool value);
		void SetAudioSource(AudioSource *source);

		void SetName(const char* name);
		const char* GetName() const;

		virtual void Update();
		virtual void UseShader(Shader *shader);

		glm::vec3 GetColorID() const;
		GameObject* IdentifyByColor(glm::vec3 colorID);

		// Hierarchy chain
		GameObject* GetParent() const;
		list<GameObject*> GetChildren() const;
		void SetParent(GameObject* object);

		void AddChild(GameObject* object);
		void RemoveChild(GameObject* object);

	public:
		// Unique unit length per color (0.0 - 1.0)

		AudioSource *audioSource;
		AABB		*aabb;
		Mesh		*mesh;
		ObjectInput	*input;
		Renderer	*renderer;
		Transform	*transform;
		Shader		*shader;
		#ifdef PHYSICS_ENGINE
		Physics		*physics;
		#endif

	protected:
		glm::vec3 colorID;
		GameObject* _parent;
		list<GameObject*> _children;

	protected:
		string name;
		const char *referenceName;
		unsigned int instanceID;
		bool debugView;
};

// TODO idea
// Create a temporary freeze state for changing various parameters and revert