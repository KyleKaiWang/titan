#version 400 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec3 v_Position;

uniform vec3 cameraPos;
uniform samplerCube u_Skybox;

void main()
{
	vec3 i = normalize(v_Position - cameraPos);
	vec3 r = reflect(i, normalize(v_Normal));
	color =  vec4(texture(u_Skybox, r).rgb, 1.0f);
}