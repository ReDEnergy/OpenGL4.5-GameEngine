#pragma once

#include <include/gl_defines.h>

class FrameBuffer;
class DirectionalLight;

class ShadowMapping {
	public:
		ShadowMapping();
		~ShadowMapping();

		void Init();
		void Update(DirectionalLight *lightSource);
		void BindTexelSize(GLint location);

	public:
		FrameBuffer *FBO;
};

