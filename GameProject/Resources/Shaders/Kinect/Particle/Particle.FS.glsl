#version 430

layout(location = 0) in vec2 text_coord;
layout(location = 1) in vec3 world_position;
layout(location = 2) in vec3 world_normal;
layout(location = 3) in vec3 v_color;

uniform sampler2D u_texture_0;
uniform sampler2D u_texture_1;

uniform vec3 eye_position;

layout(location = 0) out vec4 out_color;

vec4 PhongLight(vec3 w_pos, vec3 w_N);

void main(){
	// vec3 color = texture(u_texture_0, text_coord).xyz;
	// vec4 light = PhongLight(world_position, world_normal);
	// vec4 color = vec4(1.0, 0.5, 0, 1);
	out_color = vec4(v_color, 1.0);
}

vec3 light_position = vec3(20, 40, 30);

// -----------------------------------------------------------------------------
// Phong Light

const vec3 ld = vec3 (0.5);	// Diffuse factor
const vec3 ls = vec3 (0.9);	// Specular factor
const float specular_exponent = 40.0;	// Specular exponent
const vec3 light_dir = vec3 (0.0, -1.0, 0.0);
const float cut_off = 0.75;
 
vec4 PhongLight(vec3 w_pos, vec3 w_N)
{
	vec3 L = normalize(light_position - w_pos);
	vec3 V = normalize(eye_position - w_pos);
	vec3 H = normalize(L + V);

	float dist = distance(light_position, w_pos);

	float dot_specular = dot(w_N, L);
	vec3 specular = vec3(0);
	if (dot_specular > 0)
		specular = ls * pow(max(dot(w_N, H), 0), specular_exponent);

	vec3 diffuse = ld * max(dot_specular, 0);

	return vec4(diffuse + specular, 1.0);
}