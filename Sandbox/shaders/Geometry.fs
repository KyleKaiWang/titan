#version 450 core

layout(location = 0) out vec4 g_Position;
layout(location = 1) out vec4 g_WorldNormal;
layout(location = 2) out vec4 g_Ambient;
layout(location = 3) out vec4 g_Diffuse;
layout(location = 4) out vec4 g_Specular;

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec3 u_Ambient;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;
uniform float u_Shininess;
uniform sampler2D u_Texture;

void main()
{
	//store info to g-buffer
	g_Position = vec4(v_Position, 1.0f);
	g_WorldNormal = vec4(v_Normal, 1.0f);
	
	g_Ambient = vec4(u_Ambient, 1.0f) * texture(u_Texture, v_TexCoord);
    g_Diffuse = vec4(u_Diffuse, 1.0f) * texture(u_Texture, v_TexCoord); 
    g_Specular = vec4(u_Specular, 1.0f);
	//g_Specular.a = u_Shininess;
}