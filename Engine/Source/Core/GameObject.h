#pragma once
#include <list>
#include <string>

#include <include/glm.h>
#include <include/dll_export.h>
#include <include/utils.h>

#include <Core/Object.h>

class AABB;
class AudioSource;
class GameScript;
class Mesh;
class MeshRenderer;
class Shader;
class Physics;
class Renderer;
class Transform;
class ObjectInput;
class AnimationController;

class DLLExport GameObject: virtual public Object
{
	friend class SceneManager;
	friend class ColorPicking;
	friend class AABB;

	public:
		GameObject();
		GameObject(const char *referenceObject);
		GameObject(const GameObject &obj);
		virtual ~GameObject();

		virtual void Render(const Shader *shader) const;
		virtual void RenderTransparent(const Shader *shader) const;
		virtual void RenderInstanced(const Shader *shader, unsigned int instances) const;
		virtual void RenderDebug(const Shader *shader) const;
		virtual void RenderForPicking(const Shader *shader) const;

		void SetMesh(Mesh *mesh);
		const Mesh* GetMesh() const;
		virtual void SetDebugView(bool value);
		virtual void SetAudioSource(AudioSource *source);

		bool ColidesWith(const GameObject &object) const;
		float DistTo(const GameObject &object) const;

		void SetName(std::string name);
		const char* GetName() const;

		virtual void Update();
		virtual void LogDebugInfo() const;

		glm::vec3 GetColorID() const;
		GameObject* IdentifyByColor(const glm::vec3 &colorID);
		void SetSelectable(bool value);

		// Hierarchy chain
		GameObject* GetParent() const;
		const std::list<GameObject*>& GetChildren() const;
		unsigned int GetNumberOfChildren() const;
		void SetParent(GameObject* object);

		void AddChild(GameObject* object);
		void RemoveChild(GameObject* object);
		void RemoveChildren();

		void AddScript(GameScript *script);
		void RemoveScript(GameScript *script);
		const std::list<GameScript*>& GetScripts() const;

		void TriggerEnter(GameObject *object);
		void TriggerExit(GameObject *object);

	private:
		void Init();
		void Clear();

	protected:
		void SetupAABB();
		virtual void OnSelect() {};

	public:
		// Unique unit length per color (0.0 - 1.0)

		AudioSource *audioSource;
		AABB		*aabb;
		MeshRenderer *meshRenderer;

	public:
		Renderer	*renderer;
		Transform	*transform;
		AnimationController * animation;
		#ifdef PHYSICS_ENGINE
		Physics		*physics;
		#endif

	protected:
		const char *referenceName;
		GameObject *_parent;

		glm::vec3 colorID;
		unsigned int instanceID;
		bool debugView;
		bool selectable;

		std::string name;
		std::list<GameObject*> _children;
		std::list<GameScript*> _scripts;
};

// TODO idea
// Create a temporary freeze state for changing various parameters and revert
