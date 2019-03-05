#version 410
layout(location = 0) in vec2 texture_coord;
layout(location = 1) in vec4 world_position;
layout(location = 2) in vec3 world_normal;

uniform sampler2D u_texture_0;

uniform vec3 eye_position;
vec3 light_position = vec3(5, 0, 0);
float light_radius = 30;

layout(location = 0) out vec4 frag_color;

float PhongLight(vec3 w_pos, vec3 w_N);

// -----------------------------------------------------------------------------
// Main

void main()
{
	vec3 w_normal = normalize(world_normal);
	frag_color = texture(u_texture_0, texture_coord);
	float intensity = PhongLight(world_position.xyz, w_normal);

	frag_color = vec4(intensity);
	// frag_color = world_position;
	frag_color = vec4((w_normal + 1) * 0.5, 1);
}

// -----------------------------------------------------------------------------
// Phong Light

const float ld = 0.5;	// Diffuse factor
const float ls = 0.3;	// Specular factor
const float specular_exponent = 40.0;	// Specular exponent

float PhongLight(vec3 w_pos, vec3 w_N)
{
	vec3 L = normalize(light_position - w_pos);

	float dist = distance(light_position, w_pos);

	if (dist > light_radius)
		return 0.f;

	float att = pow(light_radius - dist, 2);

	float dot_specular = dot(w_N, L);
	float specular = 0;
	if (dot_specular > 0) {
		vec3 V = normalize(eye_position - w_pos);
		vec3 H = normalize(L + V);
		specular = ls * pow(max(dot(w_N, H), 0), specular_exponent);
	}

	float diffuse = ld * max(dot_specular, 0);

	return att * (diffuse + specular);
}