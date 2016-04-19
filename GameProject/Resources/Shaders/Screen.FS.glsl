#version 410

uniform sampler2D u_texture_0;
uniform ivec2 resolution;

layout(location = 0) out vec4 out_color;

void main()
{
	vec2 text_coord = gl_FragCoord.xy / resolution;
	out_color = texture(u_texture_0, text_coord);
}