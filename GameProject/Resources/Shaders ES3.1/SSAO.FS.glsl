#version 310 es
precision highp float;

#define V_Pos	u_texture_0
#define V_Norm	u_texture_1
#define DepthT	u_texture_2
#define Noise1	u_texture_3
#define Noise2	u_texture_4

uniform sampler2D u_texture_0;	// View positions
uniform sampler2D u_texture_1;	// View normals
uniform sampler2D u_texture_2;	// Random Texture 1
uniform sampler2D u_texture_3;	// Random Texture 2
uniform sampler2D u_texture_4;	// Depth buffer

uniform ivec2 resolution;
uniform int kernel_size;
uniform vec3 kernel[64];
uniform float u_rad;
uniform float zFar;
uniform float zNear;

uniform mat4 View;
uniform mat4 Projection;

layout(location = 0) out vec4 out_occlusion;

const float ssao_dist = 0.5;

float linearDepth(sampler2D depthTexture, vec2 coord) {
	float zt = texture(depthTexture, coord).x;
	float zn = 2.0 * zt - 1.0;
	float z = (2.0 * zNear * zFar) / (zFar + zNear - zn * (zFar - zNear));
	return z;
}

void main() {

	// -------------------------------------------------------------------------
	// Fragment properties
	
	vec2 text_coord = gl_FragCoord.xy / resolution;
	vec3 vPos	 = texture(V_Pos, text_coord).xyz;
	vec3 vNormal = texture(V_Norm, text_coord).xyz;
	float vDepth = linearDepth(DepthT, text_coord);
	
	// -------------------------------------------------------------------------
	// Random Noise vector
	
	vec2 noise_scale = resolution / 2.0;
	vec3 fres = normalize(texture(Noise2, text_coord * noise_scale).xyz);

	/////////////////////
	
	// vec3 rvec = fres;
	// vec3 tangent = normalize(rvec - vNormal * dot(rvec, vNormal));
	// vec3 bitangent = cross(vNormal, tangent);
	// mat3 tbn = mat3(tangent, bitangent, vNormal);

	/////////////
	
	// -------------------------------------------------------------------------
	
	const float strength = 1;
	const float falloff = 0.04;
	float ao = 0.0;
 	
	// -------------------------------------------------------------------------

	for(int i = 0; i < kernel_size; i++)
	{
		////////////////////
		// IQ SSAO
		
		vec3 ray = u_rad * reflect(kernel[i], fres);
		vec3 se = vPos + sign(dot(ray, vNormal)) * ray;
		// se = tbn * pSphere[i];
		se = vPos + ray;
		
		// Compute clip space coordinates
		vec4 sp = Projection * vec4(se, 1.0);
		sp.xy /= sp.w;
		sp.xy = sp.xy * 0.5 + vec2(0.5);
		
        float sd = linearDepth(DepthT, sp.xy);
		float dd = vDepth - sd; 

		// IQ attenuation	
		// float rangeCheck = dd > 0.004 ? 1.0 : 0.0;
		// float zd = rangeCheck * 20.0 * max(dd, 0.0 );
        // ao += 1.0 / (1.0 + zd * zd);
		// ao += rangeCheck;
		
		// calculate the difference between the normals as a weight
		float depthDifference = vDepth - sd;
		if (abs(depthDifference) <= ssao_dist) {
			vec3 sn = texture(V_Norm, sp.xy).xyz;
			float normDiff = (1.0 - dot(sn, vNormal));
			// the falloff equation, starts at falloff and is kind of 1/x^2 falling
			ao += step(falloff, depthDifference) * normDiff * (1.0 - smoothstep(falloff, strength, depthDifference));		
		}
	// -------------------------------------------------------------------------
   }
   
	ao = 1.0 - ao / kernel_size;
	out_occlusion = vec4(vec3(ao), 1.0);
}
