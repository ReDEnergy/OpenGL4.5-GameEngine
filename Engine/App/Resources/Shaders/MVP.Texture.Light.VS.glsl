#version 410

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_texture_coord;
layout(location = 2) in vec3 v_normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

layout(location = 0) out vec2 texture_coord;
layout(location = 1) out vec4 world_position;
layout(location = 2) out vec3 world_normal;

void main()
{
	texture_coord = v_texture_coord;

	world_normal = v_normal;
	world_position = Model * vec4(v_position, 1.0);

	gl_Position = Projection * View * world_position;
}
