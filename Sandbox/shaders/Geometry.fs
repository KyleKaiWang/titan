#version 450 core

layout(location = 1) out vec4 g_PositionTexture;
layout(location = 2) out vec4 g_NormalTexture;
layout(location = 3) out vec4 g_DiffuseTexture;

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
	//store info to g-buffer
	g_PositionTexture = vec4(v_Position, 1.0f);
	g_NormalTexture = vec4(normalize(v_Normal),1.0f);
	g_DiffuseTexture = texture( u_Texture, v_TexCoord );
}