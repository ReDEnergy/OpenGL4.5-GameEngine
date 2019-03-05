#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_texture_coord;
layout(location = 2) in vec3 v_normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

layout(location = 0) out vec4 world_normal;

void main()
{
	world_normal = vec4(v_normal, 1);
	world_normal = (normalize(world_normal) + 1) * 0.5;
	gl_Position = Projection * View * Model * vec4(v_position, 1);
}
