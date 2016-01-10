#pragma once

class World {
	public:
		World() {};
		virtual ~World() {};
		virtual void Init() {};
		virtual void FrameStart() {};
		virtual void Update(float elapsed_time, float delta_time) {};
		virtual void FrameEnd() {};
};