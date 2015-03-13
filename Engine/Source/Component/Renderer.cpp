//#include <pch.h>
#include "Renderer.h"

Renderer::Renderer() {
	castSadows = false;
}

Renderer::~Renderer() {
}

void Renderer::Update() {
}

bool Renderer::CastShadow() const {
	return castSadows;
}

void Renderer::SetCastShadow(bool value) {
	castSadows = value;
}