#pragma once
#include <list>

#include <include/dll_export.h>
#include <include/glm.h>

#include <Core/Object.h>

using namespace std;

class DLLExport Transform: virtual public Object 
{
	friend class LimitedTransform;
	friend class JointTransform;
	friend class SkinningJointTransform;

	public:
		Transform();
		Transform(const Transform &trasform);
		virtual ~Transform();

		// ****************************
		// Transform motion state

		void ClearMotionState();
		bool GetMotionState() const;

		// ****************************
		// Get transform properties

		virtual glm::vec3 GetLocalPosition() const;
		virtual glm::vec3 GetWorldPosition() const;
		virtual glm::quat GetWorldRotation() const;
		virtual glm::quat GetRelativeRotation() const;
		virtual glm::vec3 GetRotationEuler() const;
		virtual glm::vec3 GetRotationEuler360() const;

		virtual glm::vec3 GetLocalOYVector() const;
		virtual glm::vec3 GetLocalOXVector() const;
		virtual glm::vec3 GetLocalOZVector() const;

		virtual glm::vec3 GetScale() const;
		virtual const glm::mat4& GetModel() const;

		virtual float GetMoveSpeed() const;
		virtual float GetScaleSpeed() const;
		virtual float GetRotationSpeed() const;

		// ****************************
		// Continuous transform methods

		virtual void Move(const glm::vec3 offset);
		virtual void Move(const glm::vec3 dir, float deltaTime);
		virtual void Scale(float deltaTime);

		virtual void RotateLocalOY(float deltaTime);
		virtual void RotateLocalOZ(float deltaTime);
		virtual void RotateWorldOX(float deltaTime);
		virtual void RotateWorldOY(float deltaTime);
		virtual void RotateWorldOZ(float deltaTime);
		virtual void RotateLocalOX(float deltaTime);

		virtual void SetLocalPosition(glm::vec3 position);
		virtual void SetWorldPosition(glm::vec3 position);
		virtual void SetWorldRotation(glm::quat rotationQ);

		// TODO: Not implemented
		virtual void SetRelativeRotation(glm::quat relativeRotationQ);

		// Set rotation using degree angles (0 - 360)
		virtual void SetWorldRotation(glm::vec3 eulerAngles360);
		virtual void SetWorldRotationAndScale(glm::quat rotationQ, glm::vec3 scale);
		
		virtual void SetScale(glm::vec3 scale);
		virtual void ForceUpdate();

		// ****************************
		// Transform properties

		virtual void Copy(const Transform &source);
		virtual void SetMoveSpeed(float speed);
		virtual void SetScaleSpeed(float speed);
		virtual void SetRotationSpeed(float speed);

		// ****************************
		// Hierarchy chain

		void AddChild(Transform *transform);
		void RemoveChild(Transform *transform);
		void SetParent(Transform *transform);

		// ****************************
		// Transform operations
		float DistanceTo(Transform *transform);

	protected:
		virtual void Init();

		// ****************************
		// Should be called only by the owner

		virtual void UpdateWorldModel();
		virtual void UpdateWorldPosition();
		virtual void UpdateLocalPosition();
		virtual void UpdateRelativeRotation();
		virtual void UpdateChildRotation(const glm::quat &diffQ);

		virtual void UpdateChildsPosition();
		virtual void UpdateChildrenRotation(glm::quat diffQ);

	private:
		virtual void UpdateModelPosition();

	protected:
		Transform* _parentNode;
		list<Transform*> _childNodes;

		glm::vec3 _worldPosition;
		glm::quat _worldRotation;
		glm::mat4 _worldModel;

		// INFO: Relative position and location do not influence one another
		// Position relative to parent coordinate system
		glm::vec3 _localPosition;

		// Rotation relative to parent rotation
		glm::quat _relativeRotation;

		// Scale relative to the object, not influenced by rotations
		glm::vec3 _localScale;

		float _rotateSpeed;
		float _moveSpeed;
		float _scaleSpeed;
		bool _motionState;
};
