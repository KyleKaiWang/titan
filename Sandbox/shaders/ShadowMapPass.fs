#version 450 core
out vec4 Color;

void main()
{
	Color = vec4(vec3(gl_FragCoord.z), 1.0);
}