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

	public:
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
		void BindPosition(GLint location) const;
		void BindViewMatrix(GLint location) const;
		void BindProjectionMatrix(GLint location) const;
		void BindProjectionDistances(const Shader *shader) const;

		// TODO - Move this to DirectionalLight because it's related to CSM not to this implementation
		void ComputeFrustum();
		void ComputePerspectiveSection(float distance, vector<glm::vec3>::iterator it) const;
		void SplitFrustum(unsigned int splits);
		void UpdateBoundingBox(DirectionalLight * Ref) const;
		void RenderDebug(const Shader *shader) const;

		// Perspective projection
		void SetPerspective(float FOV, float aspectRatio, float zNear, float zFar);

		// Orthogonal projection
		void SetOrthgraphic(float width, float height, float zNear, float zFar);

		// Gameobject
		void SetDebugView(bool value);

	protected:
		// FPS Rotation Mode
		void UpdatePitch(float deltaAngle);
		void SetYaw(float deltaAngle);
		void UpdateRoll(float deltaAngle);

	public:
		// Camera Type
		CameraType type;

		GameObject *frustum;
		GameObject *physicalDevice;

		glm::mat4 View;
		glm::mat4 Projection;

		unsigned int splits;
		vector<float> splitDistances;
		vector<GameObject*> zones;

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

	public:
};