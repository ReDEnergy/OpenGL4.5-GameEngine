#version 310 es

layout(local_size_x = 32, local_size_y = 32) in;

#define depthBuffer u_texture_0

layout (binding = 0, rgba32f) uniform image2D diffuseMap;
layout (binding = 1, rgba32f) readonly uniform image2D worldPosition;

uniform sampler2D u_texture_0;
uniform samplerCube u_texture_cube_2;

uniform int shadowID;
uniform vec3 light_position;
uniform mat4 View;
uniform mat4 Projection;
uniform float zFar;
uniform float zNear;
uniform vec2 shadow_texel_size;
///////////////////////////////////////

ivec2 pixel;
float EvalShadow();
void BakeShadow(ivec2 pixel, float value) {
	vec4 color = imageLoad(diffuseMap, pixel);
	color.rgb *= value;
	imageStore(diffuseMap, pixel, color);
}

float ChebyshevUpperBound();
float ChebyshevUpperBound2();

///////////////////////////////////////
void main()
{
	pixel = ivec2(gl_GlobalInvocationID.xy);

	float shadow = 1.0;
	if (shadowID == 100) {
		shadow = ChebyshevUpperBound();
	} else if (shadowID == 200) {
		shadow = ChebyshevUpperBound2();
	} else {
		shadow = EvalShadow();
	}
	
	BakeShadow(pixel, shadow);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Variance Shadow Mapping

float ChebyshevUpperBound()
{
	vec4 world_position = imageLoad(worldPosition, pixel); 
	vec4 sssp = Projection * View * world_position;
	vec4 sss = sssp / sssp.w ;
	vec4 shp = sss * 0.5 + 0.5;

	if (shp.x > 1.0 || shp.y > 1.0 || shp.x < 0.0 || shp.y < 0.0 || shp.z > 1.0001) return 1.0;
	
	vec2 moments = texture(depthBuffer, shp.xy).xy;

	if (shp.z <= moments.x)
		return 1.0;

	// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
	// How likely this pixel is to be lit (p_max)
	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, 0.0001);

	float delta = shp.z - moments.x;
	float p_max = variance / (variance + delta * delta);

	return mix(0.8, 1.0, p_max);
}

float linearDepth(float depth) {
	float z = (2.0 * zNear * zFar) / (zFar + zNear - depth * (zFar - zNear));
	return z;
}

float ChebyshevUpperBound2()
{
	return 1.0;
	vec4 world_position = imageLoad(worldPosition, pixel); 
	vec3 lightDir = world_position.xyz - light_position;
	float dist = length(lightDir);
	vec2 moments = texture(u_texture_cube_2, lightDir).xy;

	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, 0.0001);

	float delta = 1.0 - moments.x;
	float p_max = variance / (variance + delta * delta);

	return mix(0.8, 1.0, p_max);
}

// -----------------------------------------------------------------------------
// Sample Cascade Shadow

vec4 shp;
float shadow_bias = 0.0001;

float IsLit(sampler2D DEPTH_TEXTURE, ivec2 offset) {
	float depth = texture(DEPTH_TEXTURE, shp.xy + vec2(offset) * vec2(shadow_texel_size)).x;
	if (depth + shadow_bias < shp.z)
		return 0.0;
	return 1.0;
}

//----------------------------------
// Percentage Closer Filtering (PCF)
float PCFfactor(sampler2D DEPTH_TEXTURE)
{
	float x, y;
	float sum = 0.0;
	float kernel_size = 3.0;
	float limit = kernel_size / 2.0 - 0.5;
	
	sum += IsLit(DEPTH_TEXTURE, ivec2(0));
	sum += IsLit(DEPTH_TEXTURE, ivec2(-limit, -limit));
	sum += IsLit(DEPTH_TEXTURE, ivec2(-limit,  limit));
	sum += IsLit(DEPTH_TEXTURE, ivec2( limit,  limit));
	sum += IsLit(DEPTH_TEXTURE, ivec2( limit, -limit));

	if (sum == 0.0)	return 0.0;
	if (sum == 5.0)	return 1.0;

	sum -= IsLit(DEPTH_TEXTURE, ivec2(0));
	
	for (y = -limit; y <= limit; y += 1.0) {
		for (x = -limit; x <= limit; x += 1.0) {
			if (abs(x) == abs(y) && abs(x) == limit)
				continue;
			sum += IsLit(DEPTH_TEXTURE, ivec2(x, y));
		}
	}
	return sum / pow(kernel_size, 2.0);
}

float EvalShadow() {

	vec4 world_position = imageLoad(worldPosition, pixel); 
	vec4 sssp = Projection * View * world_position;
	
	// perspective division
	vec4 sss = sssp / sssp.w ;
	// NDC [-1, 1] to [0, 1]
	shp = sss * 0.5 + 0.5;
	
	if (shp.x > 1.0 || shp.y > 1.0 || shp.x < 0.0 || shp.y < 0.0 || shp.z > 1.0001) return 1.0;

	float shadowFactor = PCFfactor(depthBuffer);
	return mix(0.8, 1.0, shadowFactor);
}