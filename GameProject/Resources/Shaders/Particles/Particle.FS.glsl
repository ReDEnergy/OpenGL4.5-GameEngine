#version 430
layout(location = 0) in vec2 text_coord;

uniform sampler2D u_texture_0;
uniform sampler2D u_texture_1;

layout(location = 0) out vec4 out_color;

void main(){

	vec3 color = texture(u_texture_0, text_coord).xyz;
	out_color = vec4(color, 1);
}