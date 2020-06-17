#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_TexCoord;
out vec4 v_LightSpacePosition;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Model;

uniform mat4 u_LightSpaceMatrix;

void main()
{
	v_Position = vec3(u_Model * vec4(a_Position, 1.0f));
	mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
	v_Normal = normalMatrix * a_Normal;
	v_TexCoord = a_TexCoord;
	v_LightSpacePosition = u_LightSpaceMatrix * u_Model * vec4(a_Position, 1.0f);
	
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_Model * vec4(a_Position, 1.0f);
}