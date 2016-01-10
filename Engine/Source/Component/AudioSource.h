#pragma once
#include <include/dll_export.h>
#include <include/glm.h>

#include <Core/GameObject.h>

using namespace std;

class DLLExport AudioSource
{
	public:
		AudioSource();
		AudioSource(GameObject *obj);
		virtual ~AudioSource();

		virtual void Play() {};
		virtual void Stop() {};
		virtual void SetLoop(bool value) {};
		virtual void SetVolume(unsigned int value) {};

		virtual void SetPosition(glm::vec3 position) {};

	public:
		bool sound3D;
};