#pragma once
#include <list>

#include <include/dll_export.h>
#include <include/glm.h>

#include <Core/Object.h>

using namespace std;

class DLLExport Transform: virtual public Object 
{
	friend class LimitedTransform;

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

		glm::vec3 GetLocalPosition() const;

		virtual glm::vec3 GetWorldPosition() const;
		virtual glm::quat GetWorldRotation() const;
		virtual glm::vec3 GetRotationEuler() const;

		virtual glm::vec3 GetLocalOYVector();
		virtual glm::vec3 GetLocalOXVector();
		virtual glm::vec3 GetLocalOZVector();

		virtual glm::vec3 GetScale() const;
		virtual glm::mat4 GetModel() const;

		virtual float GetMoveSpeed() const;
		virtual float GetScaleSpeed() const;
		virtual float GetRotationSpeed() const;

		// ****************************
		// Continuous transform methods

		void Move(const glm::vec3 dir, float deltaTime);
		void Scale(float deltaTime);

		void RotateLocalOY(float deltaTime);
		void RotateLocalOZ(float deltaTime);
		void RotateWorldOX(float deltaTime);
		void RotateWorldOY(float deltaTime);
		void RotateWorldOZ(float deltaTime);
		void RotateLocalOX(float deltaTime);

		virtual void SetLocalPosition(glm::vec3 position);
		virtual void SetWorldPosition(glm::vec3 position);
		virtual void SetWorldRotation(glm::quat rotationQ);

		// Set rotation using degree angles (0 - 360)
		void SetWorldRotation(glm::vec3 eulerAngles360);
		void SetWorldRotationAndScale(glm::quat rotationQ, glm::vec3 scale);

		void SetScale(glm::vec3 scale);
		void ForceUpdate();

		// ****************************
		// Transform properties

		void SetMoveSpeed(float speed);
		void SetScaleSpeed(float speed);
		void SetRotationSpeed(float speed);

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
		virtual void UpdateLocalPosition();
		virtual void UpdateLocalRotation(const glm::quat &diffQ);

	private:
		virtual void UpdatePosition();
		virtual void UpdateChildsPosition();
		virtual void UpdateChildsRotation(glm::quat diffQ);

	protected:
		Transform* _parentNode;
		list<Transform*> _childNodes;

		glm::vec3 _worldPosition;
		glm::quat _worldRotation;
		glm::mat4 _worldModel;

		glm::vec3 _localPosition;
		glm::vec3 _localScale;

		float _rotateSpeed;
		float _moveSpeed;
		float _scaleSpeed;
		bool _motionState;
};
