#version 410

#define Diffuse		u_texture_0
#define Diffuse2	u_texture_1

uniform sampler2D u_texture_0;	// Diffuse texture
uniform sampler2D u_texture_1;	// Diffuse texture

uniform vec2 resolution;

layout(location = 0) out vec3 frag_color;

vec4 GetGrayScale(vec4 color);

void main() {
	vec2 text_coord = gl_FragCoord.xy / resolution;
	vec4 diffuse = texture(Diffuse, text_coord);
	diffuse = GetGrayScale(diffuse);
	frag_color = diffuse.rgb;
}

// -----------------------------------------------------------------------------
// GrayScale Filter

vec4 GetGrayScale(vec4 color) {
	float gs = (0.21 * color.r + 0.71 * color.g + 0.07 * color.b);
	// float gs = (color.r + color.g + color.b)/3;
	return vec4(gs, gs, gs, 1.0);
}
