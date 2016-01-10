pragma once

class Transform;
class Shader;
class FrameBuffer;
class Texture;
template <class T>
class SSBO;

#include <include/glm.h>
#include <Event/EventListener.h>

class PlaneIntersection
	: public EventListener
{
	public:
		PlaneIntersection(const FrameBuffer *gameFBO);
		~PlaneIntersection() {};

		void SetPlaneTransform(const Transform* const planeTransform);

	private:
		void Update();
		void OnEvent(EventType eventID, void *data);

	private:
		SSBO<glm::ivec4> *ssbo;

		const Transform* planeTransform;
		float sphereSize;

		// Shader uniforms
		unsigned int sphere_size;
		unsigned int plane_origin;
		unsigned int plane_direction;
		Shader *computeShader;

		const FrameBuffer *gameFBO;
		Texture* visualization;
};