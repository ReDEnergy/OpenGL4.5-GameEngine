// -----------------------------------------------------------------------------
// Declare matrix GLSL

mat4 matrix = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);

// Materials

layout(std140) uniform Material
{
	vec4    mat_ambient;
	vec4    mat_diffuse;
	vec4    mat_specular;
	float   mat_shininess;
};


// -----------------------------------------------------------------------------
// General Texture Sampling Function

vec2 texel_offset = vec2(1.0/resolution);
vec4 getSample(sampler2D u_texture, vec2 text_coords, vec2 offset) {
	vec4 color = texture(u_texture, text_coords + offset * texel_offset);
	return color;
}

