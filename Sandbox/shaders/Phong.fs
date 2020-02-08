#version 400 core

layout(location = 0) out vec4 color;

in vec3 v_LightIntensity; 
uniform vec4 u_Color;

void main()
{
	color =  vec4(v_LightIntensity, 1.0f);
}