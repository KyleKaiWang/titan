#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

out vec3 v_FragPos_WS;
out vec2 v_TexCoord;
out vec4 v_FragPos_LS;
out vec3 v_T;
out vec3 v_B;
out vec3 v_N;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

void main()
{
	v_FragPos_WS = mat3(u_Model) * a_Position;
	v_TexCoord = a_TexCoord;
	
	v_T = normalize(mat3(u_Model) * a_Tangent);
    v_B = normalize(mat3(u_Model) * a_Bitangent);
    v_N = normalize(mat3(u_Model) * a_Normal);
	
	// Lights space
    v_FragPos_LS  = u_LightSpaceMatrix * vec4(v_FragPos_WS, 1.0);
	
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_Model * vec4(a_Position, 1.0f);
}