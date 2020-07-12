#version 400 core

layout(location = 0) in vec3 a_Position;

out vec3 v_TexCoord;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;

void main()
{
	v_TexCoord = a_Position;
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_Model * vec4(a_Position, 1.0f);
}