#version 450 core

layout(location = 0) out vec4 g_PositionTexture;
layout(location = 1) out vec4 g_NormalTexture;
layout(location = 2) out vec4 g_DiffuseTexture;

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;
in vec4 v_LightSpacePosition;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

float near = 0.1; 
float far  = 100.0;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	//store info to g-buffer
	g_PositionTexture = vec4(v_Position, 1.0f);
	g_NormalTexture = vec4(v_Normal,1.0f);
	g_DiffuseTexture = texture( u_Texture, v_TexCoord )* u_Color;
	//g_ShadowMap = vec4(vec3(LinearizeDepth(v_LightSpacePosition.z)/ far), 1.0f);
	//g_ShadowMap = vec4(vec3(v_LightSpacePosition.z), 1.0);
}