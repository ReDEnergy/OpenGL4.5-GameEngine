#version 430
layout(local_size_x = 32, local_size_y = 32) in;

#define depthBuffer u_texture_0

layout (binding = 0, rgba32f) uniform image2D diffuseMap;
layout (binding = 1, rgba32f) readonly uniform image2D worldPosition;

uniform sampler2D u_texture_0;

///////////////////////////////////////
// Uniforms for Cascaded Shadow Mapping 
uniform sampler2D u_texture_1;	// CSM frustum splits 0-3
uniform sampler2D u_texture_2;	// CSM frustum splits 4

uniform int shadowID;
uniform mat4[5]	CSM_LightView;
uniform mat4[5]	CSM_LightProjection;
uniform float[5] CSM_split_distance;
uniform float zFar;
uniform float zNear;

uniform vec2 shadow_texel_size;
///////////////////////////////////////

ivec2 pixel;

float EvalShadow();
float linearDepth(sampler2D depthTexture, ivec2 pixel);

void BakeShadow(ivec2 pixel, float value) {
	vec4 color = imageLoad(diffuseMap, pixel);
	color.rgb *= value;
	imageStore(diffuseMap, pixel, color);
}

///////////////////////////////////////
// VOID MAIN

void main()
{
	pixel = ivec2(gl_GlobalInvocationID.xy);
	BakeShadow(pixel, EvalShadow());
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Sample Cascade Shadow

vec4 shp;
int DEPTH_COMPONENT = 0;
int cascadeID = 0;
float shadow_bias[5] = {0.00005, 0.00014, 0.0006, 0.0012, 0.005};

int IsLit(sampler2D DEPTH_TEXTURE, ivec2 offset) {
	float depth = texture(DEPTH_TEXTURE, shp.xy + offset * shadow_texel_size)[DEPTH_COMPONENT];
	if (depth + shadow_bias[cascadeID] < shp.z)
		return 0;
	return 1;
}

//----------------------------------
// Percentage Closer Filtering (PCF)
float PCFfactor(sampler2D DEPTH_TEXTURE)
{
	// return IsLit(DEPTH_TEXTURE, ivec2( 0.0, 0.0));

	float x, y;
	float sum = 0;
	float kernel_size = 3;
	float limit = kernel_size/2 - 0.5;
	
	sum += IsLit(DEPTH_TEXTURE, ivec2( 0.0, 0.0));
	sum += IsLit(DEPTH_TEXTURE, ivec2(-limit, -limit));
	sum += IsLit(DEPTH_TEXTURE, ivec2(-limit,  limit));
	sum += IsLit(DEPTH_TEXTURE, ivec2( limit,  limit));
	sum += IsLit(DEPTH_TEXTURE, ivec2( limit, -limit));

	if (sum == 0)	return 0;
	if (sum == 5)	return 1;

	sum -= IsLit(DEPTH_TEXTURE, ivec2( 0.0, 0.0));
	
	for (y = -limit; y <= limit; y += 1.0) {
		for (x = -limit; x <= limit; x += 1.0) {
			if (abs(x) == abs(y) && abs(x) == limit)
				continue;
			sum += IsLit(DEPTH_TEXTURE, ivec2(x, y));
		}
	}
	return sum / pow(kernel_size, 2);
}

int CSM_GetCascade() {
	float depth = linearDepth(depthBuffer, pixel);

	for (int i=0; i<5; i++) {
		if (depth < CSM_split_distance[i])
			return i;
	}
	return 4;
}

float EvalShadow() {

	cascadeID = CSM_GetCascade(); 
	vec4 world_position = imageLoad(worldPosition, pixel); 
	vec4 sssp = CSM_LightProjection[cascadeID] * CSM_LightView[cascadeID] * world_position;
	
	// perspective division
	vec4 sss = sssp / sssp.w ;
	// NDC [-1, 1] to [0, 1]
	shp = sss * 0.5 + 0.5;

	DEPTH_COMPONENT = cascadeID % 4;
	float shadowFactor = cascadeID < 4 ? PCFfactor(u_texture_1) : PCFfactor(u_texture_2);
	
	return mix(0.8, 1.0, shadowFactor);
}

///////////////////////////////////////////////////////////////////////////////
// Utils

float linearDepth(sampler2D depthTexture, ivec2 pixel) {
	float zt = texelFetch(depthTexture, pixel, 0).x;
	float zn = 2.0 * zt - 1.0;
	float z = (2.0 * zNear * zFar) / (zFar + zNear - zn * (zFar - zNear));
	return z;
}
