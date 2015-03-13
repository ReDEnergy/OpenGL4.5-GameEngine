#version 410
#define HeightMap u_texture_0

// layout(triangles, equal_spacing, ccw) in;
// layout(triangles, fractional_even_spacing, ccw) in;
// layout(quads, fractional_odd_spacing, ccw) in;
layout(triangles, fractional_odd_spacing, ccw) in;

layout(location = 0) in vec2 v_texture_coord[];

uniform mat4 View;
uniform mat4 Projection;
uniform sampler2D u_texture_0;
uniform uint displacement_factor;

layout(location = 0) out vec2 texture_coord;
layout(location = 1) out vec3 world_position;

void main() {

	vec3 v_position = gl_TessCoord.x * gl_in[0].gl_Position.xyz + 
					gl_TessCoord.y * gl_in[1].gl_Position.xyz + 
					gl_TessCoord.z * gl_in[2].gl_Position.xyz;

	vec2 texcoord = gl_TessCoord.x * v_texture_coord[0] + 
					gl_TessCoord.y * v_texture_coord[1] + 
					gl_TessCoord.z * v_texture_coord[2];
					
	float displcement = texture(HeightMap, texcoord).x;

	gl_Position = Projection * View * vec4(v_position, 1.0);
	gl_Position.y += displcement * displacement_factor;

	texture_coord = texcoord;
	world_position = v_position;
}
