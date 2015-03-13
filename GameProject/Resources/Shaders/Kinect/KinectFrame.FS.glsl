#version 410
layout(location = 0) in vec2 texture_coord;

uniform sampler2D u_texture_0;

layout(location = 0) out vec4 frag_color;

void main() {
	float depth = texture(u_texture_0, texture_coord).r;
	// compute 500 - 8000 interval depth from (0-1) UINT16
	depth = depth * 8.192;
	frag_color = vec4(vec3(depth), 1.0);
}