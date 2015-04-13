#pragma once

#include <include/dll_export.h>
#include <include/glm.h>

#include <Core/Object.h>

class DLLExport Transform: virtual public Object 
{
	public:
		Transform();
		Transform(const Transform &trasform);
		Transform& operator= (const Transform &trasform);
		virtual ~Transform();
		void Update();

		void Move(const glm::vec3 dir, float deltaTime);
		void RotateYaw(float deltaTime);
		void RotatePitch(float deltaTime);
		void RotateRoll(float deltaTime);

		void SetPosition(glm::vec3 position);
		void SetRotationRadians(glm::vec3 eulerAngles);
		void SetRotation(glm::quat roationQ);
		void SetRotationAndScale(glm::quat roationQ, glm::vec3 scale);

		// Set rotation using degree angles (0 - 360)
		void SetRotation(glm::vec3 eulerAngles);
		glm::vec3 GetRotationVector();

		void SetScale(glm::vec3 scale);
		void Scale(float deltaTime);

	private:
		void UpdatePosition();

	// TODO - make them private
	public:
		glm::vec3 position;
		// angles in radians
		glm::vec3 eulerAngles;
		// rotation quaternion
		glm::quat rotationQ;
		glm::vec3 scale;
		glm::mat4 model;

		float rotateSpeed;	
		float moveSpeed;
		float scaleSpeed;
};
