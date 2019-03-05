#version 410
layout(location = 0) in vec2 texture_coord;
uniform sampler2D u_texture_0;

uniform bvec2 flip;
uniform float brightness;
uniform bool grayscale;

layout(location = 0) out vec4 out_color;

void main()
{
	vec2 text = texture_coord;
	if (flip.x == true) text.x = 1 - text.x;
	if (flip.y == true) text.y = 1 - text.y;

	out_color = texture(u_texture_0, text);

	if (brightness > 0)
	{
		out_color *= brightness;
	}
	
	if (grayscale)
	{
		out_color.gb = out_color.rr;
	}
}