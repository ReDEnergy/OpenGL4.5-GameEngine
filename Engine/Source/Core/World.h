#pragma once

class World {
	public:
		World() {};
		virtual ~World() {};
		virtual void Init() {};
		virtual void Update(float elapsed_time, float delta_time) {};
};