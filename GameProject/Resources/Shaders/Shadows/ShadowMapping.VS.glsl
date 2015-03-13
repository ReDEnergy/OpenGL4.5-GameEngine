#version 410

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_texture_coord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform mat4 LightView;
uniform mat4 LightProjection;

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec2 texture_coord;

mat4 biasMatrix = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);

void main() {
	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
	out_position = gl_Position;
	texture_coord = v_texture_coord;
	
	shadow_pos = LightProjection * LightView * Model * vec4(v_position, 1.0);
	
	// alternative - apply perspective division in vertex shader 
	shadow_pos2 = biasMatrix * shadow_pos;
	shadow_pos2 /= shadow_pos.w;	
}
