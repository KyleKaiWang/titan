#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 2) in vec2 a_TexCoord;

out vec2 v_TexCoord;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Model;
void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_Model * vec4(a_Position, 1.0f);
}