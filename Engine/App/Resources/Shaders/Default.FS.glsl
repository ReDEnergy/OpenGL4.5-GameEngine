#version 410
layout(location = 0) in vec2 texture_coord;

uniform sampler2D u_texture_0;

layout(location = 0) out vec4 frag_color;

void main() {
	frag_color = texture(u_texture_0, texture_coord);
}