#version 430
layout(location = 0) in vec2 text_coord;
layout(location = 1) in vec2 cellID;

uniform sampler2D u_texture_0;

layout(location = 0) out vec4 out_color;

void main(){

	// vec3 color = texture(u_texture_0, text_coord).xyz;
	vec3 color = vec3(cellID, 0);
	out_color = vec4(color, 1);
}