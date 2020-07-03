#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
uniform int u_Horizontal;
out vec2 v_TexCoord;
out vec2 v_BlurTexCoords[5];

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = vec4(a_Position, 1.0f);
	
	vec2 centerTexCoords = a_Position.xy * 0.5 + 0.5;
	float pixelSize = 1.0 / 1024.0;

	if(u_Horizontal == 1)
	{
		for(int i = -2; i <= 2; ++i)
		{
			v_BlurTexCoords[i + 2] = centerTexCoords + vec2(pixelSize * i, 0.0);
		}
	}
	else
	{
		for(int i = -2; i <= 2; ++i)
		{
			v_BlurTexCoords[i + 2] = centerTexCoords + vec2(0.0, pixelSize * i);
		}
	}
}