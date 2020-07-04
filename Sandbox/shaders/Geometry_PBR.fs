#version 450 core

layout(location = 0) out vec4 g_Position;
layout(location = 1) out vec4 g_WorldNormal;
layout(location = 2) out vec4 g_Albedo;
layout(location = 3) out vec4 g_Normal;
layout(location = 4) out vec4 g_MetallicRoughness;

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

//PBR
layout(binding = 0) uniform sampler2D u_Albedo;
layout(binding = 1) uniform sampler2D u_Normal;
layout(binding = 2) uniform sampler2D u_Metallic;
layout(binding = 3) uniform sampler2D u_Roughness;

void main()
{
	//store info to g-buffer
	g_Position = vec4(v_Position, 1.0f);
	g_WorldNormal = vec4(v_Normal, 1.0f);
	
	g_Albedo = texture( u_Albedo, v_TexCoord );
	g_Normal = vec4(texture( u_Normal, v_TexCoord ).rgb, 1.0f);
	g_MetallicRoughness.r = texture( u_Metallic, v_TexCoord ).r;
	g_MetallicRoughness.g = texture( u_Roughness, v_TexCoord ).r;
}