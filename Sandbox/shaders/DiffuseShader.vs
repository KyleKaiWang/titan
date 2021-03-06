#version 400 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;

uniform vec4 u_LightPosition;
uniform vec3 Kd;
uniform vec3 Ld;

out vec3 v_LightIntensity;

void main()
{
	vec3 tnorm = normalize(u_NormalMatrix * a_Normal);
	vec4 eyeCoords = u_ViewMatrix * u_Model * vec4(a_Position, 1.0f);
	vec3 s = normalize(vec3(u_LightPosition - eyeCoords));
	v_LightIntensity = Ld * Kd * max(dot(s, tnorm), 0.0f);
	
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_Model * vec4(a_Position, 1.0f);
}