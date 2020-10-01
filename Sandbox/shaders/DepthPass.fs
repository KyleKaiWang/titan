#version 400 core

in vec2 v_TexCoord;
uniform sampler2D g_Albedo;

void main()
{
	float alpha = texture(g_Albedo, v_TexCoord).a;
    if(alpha < 0.5){
        discard;
    }
}