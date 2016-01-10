#pragma once
#include <list>

#include <include/glm.h>
#include <include/dll_export.h>
#include <include/utils.h>

#include <Core/Object.h>

class AABB;
class AudioSource;
class Mesh;
class Shader;
class Physics;
class Renderer;
class Transform;
class ObjectInput;
class AnimationController;

using namespace std;

class DLLExport GameObject: virtual public Object
{
	friend class SceneManager;
	friend class AABB;

	public:
		GameObject();
		GameObject(const char *referenceObject);
		GameObject(const GameObject &obj);
		virtual ~GameObject();

		void Init();
		void Clear();

		virtual void Render() const;
		virtual void Render(const Shader *shader) const;
		virtual void RenderInstanced(const Shader *shader, unsigned int instances) const;
		virtual void RenderDebug(const Shader *shader) const;
		virtual void RenderForPicking(const Shader *shader) const;

		void SetMesh(Mesh *mesh);
		virtual void SetDebugView(bool value);
		virtual void SetAudioSource(AudioSource *source);

		void SetupAABB();
		bool ColidesWith(GameObject *object);
		float DistTo(GameObject *object) const;

		void SetName(const char* name);
		const char* GetName() const;

		virtual void Update();
		virtual void UseShader(Shader *shader);
		virtual void LogDebugInfo() const;

		glm::vec3 GetColorID() const;
		GameObject* IdentifyByColor(glm::vec3 colorID);
		void SetSelectable(bool value);

		// Hierarchy chain
		GameObject* GetParent() const;
		list<GameObject*> GetChildren() const;
		unsigned int GetNumberOfChildren() const;
		void SetParent(GameObject* object);

		void AddChild(GameObject* object);
		void RemoveChild(GameObject* object);
		void RemoveChildren();

	public:
		// Unique unit length per color (0.0 - 1.0)

		AudioSource *audioSource;
		AABB		*aabb;
		Mesh		*mesh;
		ObjectInput	*input;
		Renderer	*renderer;
		Transform	*transform;
		Shader		*shader;
		AnimationController * animation;
		#ifdef PHYSICS_ENGINE
		Physics		*physics;
		#endif

	protected:
		string name;
		glm::vec3 colorID;
		const char *referenceName;
		unsigned int instanceID;
		bool debugView;
		bool selectable;

		GameObject* _parent;
		list<GameObject*> _children;
};

// TODO idea
// Create a temporary freeze state for changing various parameters and revert