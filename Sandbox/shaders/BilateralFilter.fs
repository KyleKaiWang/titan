#version 450 core

layout(location = 0) out vec4 color;
in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D ssaoInput;
layout(binding = 1) uniform sampler2D depthTex;
layout(binding = 2) uniform sampler2D normalTex;

uniform int axis; ///< Dimension to blur along.
uniform vec2 invDstSize; ///< Inverse of the target resolution.
uniform vec2 projParams; ///< Projection depth-related coefficients.


// Fixed parameters of the Gaussian blur.
const int radius = 4; ///< Blur radius.
const int shift = 2; ///< Step between samples (introduce dithering artifacts).
const float weights[5] = float[](0.153170, 0.144893, 0.122649, 0.092902, 0.062970); ///< Gaussian weights.

//const int radius = 24;
//const float weights[25] = float[](0.003765,  0.015019,   0.023792,	0.015019,	0.003765,
//								 0.015019,	0.059912,	0.094907,	0.059912,	0.015019,
//								 0.023792,	0.094907,	0.150342,	0.094907,	0.023792,
//								 0.015019,	0.059912,	0.094907,	0.059912,	0.015019,
//								 0.003765,	0.015019,	0.023792,	0.015019,	0.003765);
								 
float linearizeDepth(float depth, vec2 projPlanes){
	float depth2 = 2.0 * depth - 1.0 ;
	return -projPlanes.y / (depth2 + projPlanes.x);
}

/** Compute bilateral weight based on depth and normal differences.
 \param n normal at the center point
 \param z depth at the center point (in view space)
 \param nNeigh normal at the neighbor
 \param zNeigh depth at the neighbor (in view space)
 \return the geometry-guided weight
 */
float computeWeight(vec3 n, float z, vec3 nNeigh, float zNeigh){
	// Depth difference.
	float zWeight = max(1.0 - abs(z - zNeigh)*2.0, 0.0);
	// Normal difference.
	float angleDiff = dot(n, nNeigh);
	angleDiff *= angleDiff;
	angleDiff *= angleDiff;
	float nWeight = max(angleDiff, 0.0);
	// Final weight.
	return nWeight * zWeight;
}

/** Compute the bilateral weight of a neighboring texel and apply it.
 \param uvNeigh the texel UV coordinates
 \param n the reference texel surface normal
 \param z the reference texel surface depth (view space)
 \param blurWeight gaussian blur weight
 \param minWeight lowest weight uncountered yet (for monotonous enforcement)
 \param sum normalization sum
 \return the weighted color sample
 */
vec4 weightedColor(vec2 uvNeigh, vec3 n, float z, float blurWeight, inout float minWeight, inout float sum){
	vec3 nNeigh = normalize(2.0 * textureLod(normalTex, uvNeigh, 0.0).rgb - 1.0);
	float zNeigh = linearizeDepth(textureLod(depthTex, uvNeigh, 0.0).r, projParams);
	// Compute weight, keep them decreasing.
	float weight = computeWeight(n, z, nNeigh, zNeigh);
	minWeight = min(minWeight, weight);
	// Blend with Gaussian weight.
	float totalWeight = blurWeight * minWeight;
	sum += totalWeight;
	vec4 colNeigh = textureLod(ssaoInput, uvNeigh, 0.0);
	return totalWeight * colNeigh;
}

/** Apply an approximate bilateral filter combining a separable gaussian blur and a geometry-guided weight.
 */
void main(){

	// Center always contribute.
	vec4 col = textureLod(ssaoInput, v_TexCoord, 0.0);
	float sum = weights[0];
	vec4 accum = sum * col;
	// Get reference geoemtric info.
	vec3 n = normalize(2.0 * textureLod(normalTex, v_TexCoord, 0.0).rgb - 1.0);
	float z = linearizeDepth(textureLod(depthTex, v_TexCoord, 0.0).r, projParams);

	// Delta is a one pixel shift of the target resolution.
	vec2 delta = axis == 0 ? vec2(shift * invDstSize.x, 0.0) : vec2(0.0, shift * invDstSize.y);

	// Explore on both sides, avoid non monotonous weights.
	float minWeight = 1000000.0;
	for(int dx = 1; dx <= radius; ++dx){
		vec2 uvNeigh = v_TexCoord - dx * delta;
		accum += weightedColor(uvNeigh, n, z, weights[dx], minWeight, sum);
	}

	minWeight = 1000000.0;
	for(int dx = 1; dx <= radius; ++dx){
		// Shift UV and read geometric info.
		vec2 uvNeigh = v_TexCoord + dx * delta;
		accum += weightedColor(uvNeigh, n, z, weights[dx], minWeight, sum);
	}

	// Final normalization.
	color = accum;
	if(sum != 0.0){
		color /= sum;
	}
}