#pragma once
#include <Prototyping/Prototype.h>

class FrameBuffer;
class Texture;

class ToFSimulation :
	public Prototype
{
	public:
		ToFSimulation();
		~ToFSimulation() {};

		void Init(FrameBuffer &frameBuffer);

	private:
		void Update();

		// Listen for mouse and keyboard events
		void OnKeyPress(int key, int mods);

		// Listen for Events
		void OnEvent(EventType Event, void *data);

	private:
		FrameBuffer *FBO;
		Texture *computeTexture;
};