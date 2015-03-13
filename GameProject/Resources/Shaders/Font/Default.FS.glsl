#version 410
layout(location = 0) in vec2 texture_coord;

uniform sampler2D u_texture_0;
uniform vec3 text_color;

layout(location = 0) out vec4 frag_color;

void main() {
    float dist = texture2D(u_texture_0, texture_coord).r;
    float width = fwidth(dist);
    float alpha = smoothstep(0.5-width, 0.5+width, dist);
    frag_color = vec4(text_color, alpha);

//	vec4 color = vec4(0.967, 0.333, 0.486, 1.0);
}