#version 450 core

#define BLUR_HORIZ 0
#define BLUR_VERT 1
#define RADIUS 4

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
layout(binding = 0) uniform sampler2D ssaoInput;
uniform int filter_scale;
uniform float edge_sharpness;
uniform ivec2 axis;
const float gaussian[RADIUS + 1] = float[](0.153170, 0.144893, 0.122649, 0.092902, 0.062970);

void main()
{
    ivec2 px = ivec2(gl_FragCoord.xy);
	vec3 val = texelFetch(ssaoInput, px, 0).xyz;
	float z_pos = val.y;

	// Compute weighting for the term at the center of the kernel
	float base = gaussian[0];
	float weight = base;
	float sum = weight * val.x;

	for (int i = -RADIUS; i <= RADIUS; ++i){
		// We handle the center pixel above so skip that case
		if (i != 0){
			// Filter scale effects how many pixels the kernel actually covers
			ivec2 p = px + axis * i * filter_scale;
			vec3 val = texelFetch(ssaoInput, p, 0).xyz;
			float z = val.y;
			float w = 0.3 + gaussian[abs(i)];
			// Decrease weight as depth difference increases. This prevents us from
			// blurring across depth discontinuities
			w *= max(0.f, 1.f - (edge_sharpness * 400.f) * abs(z_pos - z));
			sum += val.x * w;
			weight += w;
		}
	}
	color = vec4(vec2(sum / (weight + 0.0001), val.y), 1.0, 1.0);
}