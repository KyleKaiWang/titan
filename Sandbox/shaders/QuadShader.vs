#version 400 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
//layout(location = 2) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec2 v_TexCoord;
//out vec3 v_Normal;

void main()
{
	v_TexCoord = a_TexCoord;
	//v_Normal = a_Normal;
	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0f);
}