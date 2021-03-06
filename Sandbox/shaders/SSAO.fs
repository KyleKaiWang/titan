#version 450 core

layout(location = 0) out vec4 color;
#define FAR_PLANE -100.f
in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;

uniform int screenWidth;
uniform int screenHeight;

uniform int use_rendered_normals;
uniform int n_samples;
uniform int turns;
uniform float ball_radius;
uniform float sigma;
uniform float kappa;
uniform float beta;

uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection;

void main(void)
{
	vec3 gPos = texture(gPosition, v_TexCoord).rgb;
	vec2 viewport_dim = vec2(screenWidth, screenHeight);
	
	vec3 pos = vec3(u_ViewMatrix * vec4(gPos, 1.0));
	vec3 normal;
	vec3 camera_normals = vec3(u_ViewMatrix * texture(gNormal, gl_FragCoord.xy / viewport_dim));
	if (use_rendered_normals == 1){
		//normal = normalize(texture(camera_normals, gl_FragCoord.xy / viewport_dim).xyz);
		normal = camera_normals;
	}
	else {
		normal = normalize(cross(dFdx(pos), dFdy(pos)));
	}

	// The Alchemy AO hash for random per-pixel offset
	ivec2 px = ivec2(gl_FragCoord.xy);
	float phi = (3 * px.x ^ px.y + px.x * px.y) * 10;
	
	const float TAU = 6.2831853071795864;
	const float ball_radius_sqr = pow(ball_radius, 2);
	
	// What's the radius of a 1m object at z = -1m to compute screen_radius properly?
	// Comments in their code mention we can compute it from the projection mat, or hardcode in like 500
	// and make the ball radius resolution dependent (as I've done currently)
	const float screen_radius = -ball_radius * 3500 / pos.z;
	int max_mip = textureQueryLevels(gPosition) - 1;
	float ao_value = 0;
	for (int i = 0; i < n_samples; ++i){
		float alpha = 1.f / n_samples * (i + 0.5);
		float h = screen_radius * alpha;
		float theta = TAU * alpha * turns + phi;
		vec2 u = vec2(cos(theta), sin(theta));
		int m = clamp(findMSB(int(h)) - 4, 0, max_mip);
		ivec2 mip_pos = clamp((ivec2(h * u) + px) >> m, ivec2(0), textureSize(gPosition, m) - ivec2(1));
		vec3 q = vec3(u_ViewMatrix * vec4(texelFetch(gPosition, mip_pos, m).xyz, 1.0f));
		vec3 v = q - pos;
		// The original estimator in the paper, from Alchemy AO
		// I tried getting their new recommended estimator running but couldn't get it to look nice,
		// from taking a look at their AO shader it also looks like we compute this value quite differently
		ao_value += max(0, dot(v, normal + pos.z * beta)) / (dot(v, v) + 0.01);
	}
	// The original method in paper, from Alchemy AO
	ao_value = max(0, 1.f - 2.f * sigma / n_samples * ao_value);
	ao_value = pow(ao_value, kappa);

    // Do a little bit of filtering now, respecting depth edges
    if (abs(dFdx(pos.z)) < 0.02) {
        ao_value -= dFdx(ao_value) * ((px.x & 1) - 0.5);
    }
    if (abs(dFdy(pos.z)) < 0.02) {
        ao_value -= dFdy(ao_value) * ((px.y & 1) - 0.5);
    }
	//color = vec4(vec2(ao_value, pos.z / FAR_PLANE), 1.0, 1.0);
	color = vec4(ao_value, ao_value, ao_value, 1.0);
}