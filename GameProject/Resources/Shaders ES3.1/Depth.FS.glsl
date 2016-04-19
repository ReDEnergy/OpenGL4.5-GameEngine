#version 310 es
precision highp float;
layout(location = 0) in vec2 texture_coord;

uniform sampler2D u_texture_0;	

void main() {
	vec4 diffuse = texture(u_texture_0, texture_coord);
	if(diffuse.a < 0.9)
		discard;
}