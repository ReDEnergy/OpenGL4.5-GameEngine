#pragma once
#include <vector>

#include <include/dll_export.h>
#include <include/glm.h>

#include <Core/GameObject.h>

enum class CameraType {
	FirstPerson,
	ThirdPerson
};

class AABB;
class DirectionalLight;
class FrustumSplit;

struct ProjectionInfo
{
	float FoV;
	float zNear;
	float zFar;
	float aspectRatio;
	float width;
	float height;
	bool isPerspective;
};

class DLLExport Camera: virtual public GameObject {
	public:
		Camera();
		~Camera();

		void Init();
		void Log() const;
		virtual void Update();

	public:
		glm::mat4 GetViewMatrix() const;

		// Rotation
		void RotateOX(float deltaTime);
		void RotateOY(float deltaTime);
		void RotateOZ(float deltaTime);

		// Speed
		void UpdateSpeed(float offset = 0.2f);

		// Translate Camera - local axis
		void SetPosition(glm::vec3 position);

		// Translate Camera - world axis
		void MoveForward(float deltaTime);
		void MoveBackward(float deltaTime);
		void MoveRight(float deltaTime);
		void MoveLeft(float deltaTime);
		void MoveUp(float deltaTime);
		void MoveDown(float deltaTime);
		void MoveInDirection(glm::vec3 direction, float deltaTime);

		// Bind shader uniforms
		void BindPosition(int location) const;
		void BindViewMatrix(int location) const;
		void BindProjectionMatrix(int location) const;
		void BindProjectionDistances(const Shader *shader) const;

		void RenderDebug(const Shader *shader) const;

		void SetPerspective(float FOV, float aspectRatio, float zNear, float zFar);
		void SetOrthgraphic(float width, float height, float zNear, float zFar);
		void SetProjection(const ProjectionInfo &PI);
		ProjectionInfo GetProjectionInfo() const;

		bool ColidesWith(GameObject *object);

	private:
		void ComputeFrustum();

	protected:
		// FPS Rotation Mode
		void UpdatePitch(float deltaAngle);
		void SetYaw(float deltaAngle);
		void UpdateRoll(float deltaAngle);

	public:
		// Camera Type
		CameraType type;
		GameObject *frustum;

		glm::mat4 View;
		glm::mat4 Projection;

	protected:
		// Controll settings
		float minSpeed;
		float maxSpeed;
		float sensitivityOX;
		float sensitivityOY;

		// Rotation OX constrains
		float limitUp;
		float limitDown;

		// Perspective properties
		float zNear;
		float zFar;
		float FOV;
		float aspectRatio;
		bool isPerspective;
		float ortographicWidth;
};