#version 410
layout(location = 0) in vec4 frag_pos;
layout(location = 1) in vec2 texture_coord;

uniform sampler2D u_texture_0;	

layout(location = 0) out vec4 VSM;

void main() {
	vec4 diffuse = texture(u_texture_0, texture_coord);
	if(diffuse.a < 0.9)
		discard;
		
	float depth = frag_pos.z / frag_pos.w;
	depth = depth * 0.5 + 0.5;

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float moment2 = depth * depth + 0.25*(dx*dx + dy*dy) ;
	
	VSM[0] = depth;
	VSM[1] = moment2;
}