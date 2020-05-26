#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D g_PositionTexture;
uniform sampler2D g_NormalTexture;
uniform sampler2D g_DiffuseTexture;

uniform vec4 u_LightPosition;
uniform float u_LightIntensity;

vec3 diffuseModel(vec3 pos, vec3 normal, vec3 diff)
{
	vec3 s = normalize(vec3(u_LightPosition) - pos);
	float sDotN = max(dot(s, normal), 0.0);
	vec3 diffuse = u_LightIntensity * diff * sDotN;
	return diffuse;
}

void main()
{
	//Get info from the g-buffer textures
	vec3 pos = vec3( texture( g_PositionTexture, v_TexCoord ) );
    vec3 normal = vec3( texture( g_NormalTexture, v_TexCoord ) );
    vec3 diffuseColor = vec3( texture(g_DiffuseTexture, v_TexCoord) );

    color = vec4( diffuseModel(pos,normal,diffuseColor), 1.0 );
}