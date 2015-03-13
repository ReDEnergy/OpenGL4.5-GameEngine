#version 410

#define Diffuse	u_texture_0
uniform sampler2D u_texture_0;

uniform ivec2 resolution;

layout(location = 0) out vec3 color;

vec4 getBlur(sampler2D u_texture, vec2 text_coords);

void main() {
	vec2 text_coord = gl_FragCoord.xy / resolution;
	color = texture(u_texture_0, text_coord).xyz;
	color = getBlur(Diffuse, text_coord).xyz;
}

const float uBlurSize = 4;

vec4 getBlur(sampler2D u_texture, vec2 text_coords) {

	vec2 texelSize = 1.0 / vec2(textureSize(u_texture, 0));
	float result = 0.0;
	vec2 hlim = vec2(float(-uBlurSize) * 0.5 + 0.5);
	for (int x = 0; x < uBlurSize; ++x) {
		for (int y = 0; y < uBlurSize; ++y) {
			vec2 offset = (hlim + vec2(float(x), float(y))) * texelSize;
			result += texture(u_texture, text_coords + offset).r;
		}
	}
 
	float rez = result / float(uBlurSize * uBlurSize);
	return vec4(rez, rez, rez, 1.0);
}

// -----------------------------------------------------------------------------
// Texture Sampling

vec2 texel_offset = vec2(1.0/resolution);
vec4 getSample(sampler2D u_texture, vec2 text_coords, vec2 offset) {
	vec4 color = texture(u_texture, text_coords + offset * texel_offset);
	return color;
}