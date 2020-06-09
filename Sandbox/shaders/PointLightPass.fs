#version 450 core

layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec2 v_TexCoord;

uniform sampler2D g_PositionTexture;
uniform sampler2D g_NormalTexture;
uniform sampler2D g_DiffuseTexture;

struct PointLight {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
	float Radius;
};
const int NR_LIGHTS = 5;
uniform PointLight u_Lights[NR_LIGHTS];
uniform vec3 u_ViewPos;

vec3 CalcPointLight(PointLight light, vec3 viewDir, vec3 gNormal, vec3 gPos, vec3 gColor, float gSpec, float brightness);

void main() 
{ 
	//From g-buffers
	vec3 pos = vec3( texture( g_PositionTexture, v_TexCoord ) );
    vec3 normal = vec3( texture( g_NormalTexture, v_TexCoord ) );
    vec3 diffuseColor = vec3( texture(g_DiffuseTexture, v_TexCoord) );
    float specular = texture(g_DiffuseTexture, v_TexCoord).a;
	
    vec3 viewDir = normalize(u_ViewPos - pos);
	
	vec3 result = vec3(0.0f);
	for(int i = 0; i < NR_LIGHTS; ++i) {
        float distance = length(u_Lights[i].Position - pos);
		if(distance < u_Lights[i].Radius)
			result += u_Lights[i].Color * CalcPointLight(u_Lights[i], viewDir, normal, pos, diffuseColor, specular, 10.0f);
	}
	color = vec4(vec4(result, 1.0) * vec4(diffuseColor, 1.0));
}

vec3 CalcPointLight(PointLight light, vec3 viewDir, vec3 gNormal, vec3 gPos, vec3 gColor, float gSpec, float brightness)
{
    vec3 lightDir = normalize(light.Position - gPos);
	
    // diffuse shading
    float diff = max(dot(gNormal, lightDir), 0.0);
	
    // specular shading
    vec3 reflectDir = reflect(-lightDir, gNormal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
	
    // attenuation
    float distance = length(light.Position - gPos);
    float attenuation = brightness / (1.0 + light.Linear * distance + light.Quadratic * (distance * distance));
    
    // combine results
    vec3 ambient = gColor;
    vec3 diffuse = diff * gColor;
    vec3 specular = spec * vec3(gSpec);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

