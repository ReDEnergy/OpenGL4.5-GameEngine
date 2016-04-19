#pragma once

class World {
	public:
		World() {};
		virtual ~World() {};
		virtual void Init() {};
		virtual void FrameStart() {};
		virtual void Update(float deltaTime) {};
		virtual void FrameEnd() {};
};