#version 450 core
#define MAX_POINTLIGHTS 128
layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D g_Position;
layout(binding = 1) uniform sampler2D g_WorldNormal;

// Phong
layout(binding = 2) uniform sampler2D g_Ambient;
layout(binding = 3) uniform sampler2D g_Diffuse;
layout(binding = 4) uniform sampler2D g_Specular;

struct PointLight {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
	float Radius;
};

uniform int u_LightNum;
uniform PointLight u_Lights[MAX_POINTLIGHTS];
uniform vec3 u_ViewPos;

vec3 CalcPointLight(PointLight light, vec3 viewDir, vec3 gPos, vec3 gNormal, vec3 g_Ambient, vec3 gDiffuse, vec3 gSpec, float brightness)
{
    vec3 lightDir = normalize(light.Position - gPos);
	
    // diffuse shading
    float diff = max(dot(gNormal, lightDir), 0.0);
	
    // specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(gNormal, halfwayDir), 0.0), 32.0);
	
    // attenuation
    float distance = length(light.Position - gPos);
    float attenuation = brightness / (1.0 + light.Linear * distance + light.Quadratic * (distance * distance));
    
    // combine results
    vec3 diffuse = diff * gDiffuse * light.Color;
    vec3 specular = gSpec * light.Color;
    diffuse *= attenuation;
    specular *= attenuation;
    return diffuse + specular;
}

void main() 
{ 
	//From g-buffers
	vec3 pos = texture( g_Position, v_TexCoord ).rgb;
    vec3 normal = texture( g_WorldNormal, v_TexCoord ).rgb;
	vec3 ambient = texture(g_Ambient, v_TexCoord).rgb;
    vec3 diffuse = texture(g_Diffuse, v_TexCoord).rgb;
    vec3 specular = texture(g_Specular, v_TexCoord).rgb;
	
    vec3 viewDir = normalize(u_ViewPos - pos);
	
	vec3 result = vec3(0.0);
	for(int i = 0; i < u_LightNum; ++i) {
        float distance = length(u_Lights[i].Position - pos);
		//if(distance < u_Lights[i].Radius)
		result += CalcPointLight(u_Lights[i], viewDir, pos, normal, ambient, diffuse, specular, 1.0f);
	}
	color = vec4(result, 1.0);
}

