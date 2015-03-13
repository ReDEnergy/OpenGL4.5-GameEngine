#version 410

#define Diffuse	u_texture_0
uniform sampler2D u_texture_0;	// Diffuse texture
uniform vec2 resolution;

layout(location = 0) out vec4 frag_color;

vec4 getGuassian(sampler2D u_texture, vec2 text_coords);

void main() {
	vec2 text_coord = gl_FragCoord.xy / resolution;
	// vec4 diffuse = texture(Diffuse, text_coord);
	vec4 diffuse = getGuassian(Diffuse, text_coord);
	frag_color = diffuse;
}

// -----------------------------------------------------------------------------
// Texture Sampling

vec2 texel_offset = vec2(1.0/resolution);
vec4 getSample(sampler2D u_texture, vec2 text_coords, vec2 offset) {
	vec4 color = texture(u_texture, text_coords + offset * texel_offset);
	return color;
}

// -----------------------------------------------------------------------------
// Gaussian Filter

const float gkf = 159.0;
const float gkrow1[5] = float[](2/gkf,  4/gkf,  5/gkf,  4/gkf, 2/gkf);
const float gkrow2[5] = float[](4/gkf,  9/gkf, 12/gkf,  9/gkf, 4/gkf);
const float gkrow3[5] = float[](5/gkf, 12/gkf, 15/gkf, 12/gkf, 5/gkf);
const float g_kernel[25] = float[](
	2/gkf,  4/gkf,  5/gkf,  4/gkf, 2/gkf,
	4/gkf,  9/gkf, 12/gkf,  9/gkf, 4/gkf,
	5/gkf, 12/gkf, 15/gkf, 12/gkf, 5/gkf,
	4/gkf,  9/gkf, 12/gkf,  9/gkf, 4/gkf,
	2/gkf,  4/gkf,  5/gkf,  4/gkf, 2/gkf
);

vec4 getGuassian(sampler2D u_texture, vec2 text_coords) {
	int x, y; 
	vec4 color = vec4(0.0);
	for (y = 0; y < 5; y += 1) {
		for (x = 0; x < 5; x += 1) {
			color += g_kernel[y * 5 + x] * getSample(u_texture, text_coords, vec2(x - 2, y - 2));
		}
	}
	return color;
}
