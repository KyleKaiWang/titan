#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform int screenWidth;
uniform int screenHeight;

uniform int kernelSize = 64;
uniform float radius = 0.5;
uniform float bias = 0.025;

uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection;

void main()
{
    // get input for SSAO algorithm
    vec3 fragPos = vec4(u_ViewMatrix * texture(gPosition, v_TexCoord)).rgb;
    vec3 normal = normalize(texture(gNormal, v_TexCoord).rgb);
	
	// tile noise texture over screen based on screen dimensions divided by noise size
	const vec2 noiseScale = vec2(screenWidth/4.0, screenHeight/4.0); 
    vec3 randomVec = normalize(texture(texNoise, v_TexCoord * noiseScale).rgb);
	
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
	
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = u_Projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = vec4(u_ViewMatrix * texture(gPosition, offset.xy)).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    color = vec4(vec3(occlusion), 1.0f);
}