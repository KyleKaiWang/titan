#version 450 core
layout (location = 0) out vec4 color;
in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D scene;
layout(binding = 1) uniform sampler2D sceneBloomBlur;
uniform bool enableBloom;
uniform float exposure;
uniform float sceneFactor;
uniform float bloomFactor;

void main()
{
	const float gamma = 2.2;
    vec3 hdrColor = texture(scene, v_TexCoord).rgb * sceneFactor;      
    vec3 bloomColor = texture(sceneBloomBlur, v_TexCoord).rgb * bloomFactor;
    
	// additive blending
	if(enableBloom)
        hdrColor += bloomColor;
		
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    color = vec4(result, 1.0);
}
