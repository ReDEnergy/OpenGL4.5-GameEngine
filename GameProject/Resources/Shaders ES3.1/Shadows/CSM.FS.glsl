#version 310 es
precision highp float;
layout(location = 0) in vec4 frag_pos;
layout(location = 1) in vec2 texture_coord;

uniform sampler2D u_texture_0;	
uniform int CSM_cascadeID;	// cascade ID

layout(location = 0) out vec4 CSM;

void main() {
	vec4 diffuse = texture(u_texture_0, texture_coord);
	if(diffuse.a < 0.9)
		discard;
		
	if (CSM_cascadeID < 4) {

		float depth = frag_pos.z / frag_pos.w;
		depth = depth * 0.5 + 0.5;

		CSM = vec4(0);
		CSM[CSM_cascadeID] = depth;
	}
}