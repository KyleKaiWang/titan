#version 400 core

layout(location = 0) out vec4 color;

in vec3 v_TexCoord;
uniform samplerCube u_Skybox;

void main()
{
	color =  texture(u_Skybox, v_TexCoord);
}