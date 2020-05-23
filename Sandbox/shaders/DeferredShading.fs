#version 400 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec3 positionData;
layout(location = 2) out vec3 normalData;
layout(location = 3) out vec3 colorData;

in vec3 v_Position;
in vec2 v_TexCoord;
in vec3 v_Normal;

struct LightInfo
{
	vec3 position;
	vec3 Color;
	float Intensity;
};

uniform sampler2D u_PositionTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_ColorTexture;
uniform LightInfo u_Light;
uniform vec3 u_Ambient;

subroutine void RenderPassType(); 
subroutine uniform RenderPassType RenderPass; 

vec3 diffuseModel(vec3 pos, vec3 normal, vec3 diff)
{
	vec3 s = normalize(vec3(u_Light.position) - pos);
	float sDotN = max(dot(s, normal), 0.0);
	vec3 diffuse = u_Light.Intensity * diff * sDotN;
	return diffuse;
}

subroutine (RenderPassType)
void pass1()
{
	//store info to g-buffer
	positionData = v_Position;
	normalData = v_Normal;
	colorData = u_Ambient;
}

subroutine(RenderPassType)
void pass2()
{
	//Get info from the g-buffer textures
	vec3 pos = vec3( texture( u_PositionTexture, v_TexCoord ) );
    vec3 normal = vec3( texture( u_NormalTexture, v_TexCoord ) );
    vec3 diffuseColor = vec3( texture(u_ColorTexture, v_TexCoord) );

    color = vec4( diffuseModel(pos,normal,diffuseColor), 1.0 );
}

void main()
{
	RenderPass();
}