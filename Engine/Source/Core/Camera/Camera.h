#pragma once
#include <vector>

#include <include/dll_export.h>
#include <include/gl.h>
#include <include/glm.h>

#include <Core/GameObject.h>

using namespace std;

enum class CameraType {
	FirstPerson,
	ThirdPerson
};

class AABB;
class DirectionalLight;

class DLLExport Camera: virtual public GameObject {
	public:
		Camera();
		~Camera();

		void Init();
		void Log() const;
		virtual void Update();

		// FPS Rotation Mode
		void UpdatePitch(float deltaAngle);
		void SetYaw(float deltaAngle);
		void UpdateRoll(float deltaAngle);

		// Rotation
		void RotateOX(float deltaTime);
		void RotateOY(float deltaTime);
		void RotateOZ(float deltaTime);

		void SetDirection(glm::vec3 direction);

		// Speed
		void IncreaseSpeed();
		void DecreaseSpeed();

		// Translate Camera - local axis
		void Set(glm::vec3 position, glm::vec3 target, glm::vec3 up);
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
		void BindPosition(GLint location) const;
		void BindViewMatrix(GLint location) const;
		void BindProjectionMatrix(GLint location) const;
		void BindProjectionDistances(const Shader *shader) const;

		void UpdateBoundingBox(DirectionalLight *Ref);
		void UpdateDebug();

		void ComputeFrustum();
		void ComputePerspectiveSection(float distance, vector<glm::vec3>::iterator it) const;
		void SplitFrustum(unsigned int splits);
		void RenderDebug(const Shader *shader) const;

		// Perspective projection
		void SetPerspective(float FOV, float aspectRatio, float zNear, float zFar);

		// Orthogonal projection
		void SetOrthgraphic(float width, float height, float zNear, float zFar);

	public:
		// Camera Type
		CameraType type;

		// Rotation OX constrains
		float limitUp;
		float limitDown;

		// Speed
		float minSpeed;
		float maxSpeed;
		float sensivityOX;
		float sensivityOY;

		GameObject *frustum;
		GameObject *physicalDevice;

		glm::mat4 View;
		glm::mat4 Projection;
		glm::vec3 forward;

		float zNear;
		float zFar;
		float FOV;
		float aspectRatio;

	protected:
		glm::vec3 right;
		glm::vec3 up;

	public:
		unsigned int splits;
		vector<float> splitDistances;
		vector<GameObject*> zones;
};