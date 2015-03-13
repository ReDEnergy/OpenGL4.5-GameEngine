uniform vec3 eye_position;
uniform vec3 light_color;

// -----------------------------------------------------------------------------
// Phong Light

const vec3 ld = vec3 (1.0, 1.0, 1.0);	// Diffuse factor
const vec3 ls = vec3 (1.0, 1.0, 1.0);	// Specular factor
const float specular_exponent = 40.0;	// Specular exponent
const vec3 light_dir = vec3 (0.0, -1.0, 0.0);
const float cut_off = 0.75;
 
vec4 PhongLight(vec3 w_pos, vec3 w_N)
{
	vec3 L = normalize(light_position - w_pos);
	vec3 V = normalize(eye_position - w_pos);
	vec3 H = normalize(L + V);

	// attenuation factor
	float att_power = 30;
	float d = distance(light_position, w_pos) / att_power;
	float att = 1.0f / pow(d, 2);
	
	float dot_specular = dot(w_N, L);
	vec3 specular = vec3(0);
	
	if (dot_specular > 0)
		specular = ls * pow(max(dot(w_N, H), 0), specular_exponent);

	vec3 diffuse = ld * max(dot_specular, 0);

	return vec4(att * (diffuse + specular), 1.0);
}

vec4 CalcSpotLight(vec3 w_pos, vec3 w_N)
{
	vec3 light_direction  = normalize(w_pos - light_position);
	float spot_factor = dot(light_direction, light_dir);

	if (spot_factor > cut_off) {
		vec4 color = PhongLight(w_pos, w_N);
		return color * (1.0 - (1.0 - spot_factor) / (1.0 - cut_off));
	}
	return vec4(0, 0, 0, 0);
}