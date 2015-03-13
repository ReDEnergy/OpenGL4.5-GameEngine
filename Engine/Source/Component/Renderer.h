#pragma once
#include <Core/Object.h>

/*
 * Class Renderer
 */

class Renderer: virtual public Object {
	public:
		Renderer();
		virtual ~Renderer();
		virtual void Update();

		void SetCastShadow(bool value);
		bool CastShadow() const;

	private:
		bool castSadows;
};

