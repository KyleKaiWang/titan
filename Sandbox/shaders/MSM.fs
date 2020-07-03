#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D g_Position;
layout(binding = 1) uniform sampler2D g_WorldNormal;
layout(binding = 2) uniform sampler2D g_Ambient;
layout(binding = 3) uniform sampler2D g_Diffuse;
layout(binding = 4) uniform sampler2D g_Specular;
layout(binding = 5) uniform sampler2D g_ShadowMap;
layout(binding = 6) uniform sampler2D g_ShadowMap_H;
layout(binding = 7) uniform sampler2D g_ShadowMap_V;

uniform vec3 u_ViewPos;
uniform mat4 u_LightSpaceMatrix;
uniform vec3 u_LightPos;
uniform vec3 u_LightDir;
uniform vec3 u_LightAttenuation;
uniform vec3 u_LightDiffuse;
uniform vec3 u_LightSpecular;
uniform float u_LightCutOff;
uniform float u_LightOuterCutOff;

vec3 CholeskyDecomposition(float m11, float m12, float m13, float  m22, float m23, float m33, float z1, float z2, float z3)
{
	float a = sqrt(m11);
	float b = m12 / a;
	float c = m13 / a;
	float d = sqrt(m22 - (b * b));
	float e = (m23 - (b * c)) / d;
	float f = sqrt(m33 - (c * c) - (e * e));

	float c1_cap = z1 / a;
	float c2_cap = (z2 - (b * c1_cap)) / d;
	float c3_cap = (z3 - (c * c1_cap) - (e * c2_cap)) / f;

	vec3 result;
	result.z = c3_cap / f;
	result.y = (c2_cap - (e * result.z)) / d;
	result.x = (c1_cap - (b * result.y) - (c * result.z)) / a;
	return result;
}

float ShadowIntensity(vec4 fragPosLightSpace) {	
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
	
	// current depth
	float z_f = projCoords.z;		
	
	// for directional light
	float alpha = 0.001;
	
	// this needs adjustment with position of light - for point light
	//float alpha = 0.055; 
	
	vec4 b = texture(g_ShadowMap_V, projCoords.xy);
	vec4 b_prime = (1.0 - alpha) * b + alpha * vec4(0.5);
	vec3 c = CholeskyDecomposition(1.0, b_prime.x, b_prime.y, b_prime.y, b_prime.z, b_prime.w, 1.0, z_f, z_f * z_f);
	
	float c3 = c.z; // a
	float c2 = c.y; // b
	float c1 = c.x; // c
	float det = c2 * c2 - 4 * c3 * c1;
	if(det < 0)
	{
		return 0.0;
	}
	
	det = sqrt(det);
	float z_2 = (-c2 - det) / (2 * c3);
	float z_3 = (-c2 + det) / (2 * c3);

	// enable this for direction light 
	// disable this for point light
	if(z_3 < z_2)
	{
		float temp = z_3;
		z_3 = z_2;
		z_2 = temp;
	}
	
	float bias = 0.000;
	float G;
	if(z_f <= z_2)
	{
		G = 0.0;
	}
	else if(z_f<= z_3)
	{
		G = (z_f * z_3 - b_prime.x * (z_f + z_3) + b_prime.y) / ((z_3 - z_2) * (z_f - z_2));
	}
	else
	{
		G = 1.0 - ((z_2 * z_3 - b_prime.x * (z_2 + z_3) + b_prime.y) / ((z_f - z_2) * (z_f - z_3)));
	}
	
	return G;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 g_WorldNormal)
{
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
    // transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	
	float bias = max(0.0001 * (1.0 - dot(g_WorldNormal, u_LightDir)), 0.0000001);
    float closestDepth = texture(g_ShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z - bias;
	
    // check whether current frag pos is in shadow
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    
	return shadow;
}

// calculates the color when using a directional light.
vec3 CalcDirLight(vec3 gPos, vec3 gNormal, vec3 gAmbient, vec3 gDiffuse, vec3 gSpec, float gShininess)
{
    vec3 lightDir = normalize(-u_LightDir);
	vec3 viewDir  = normalize(u_ViewPos - gPos);
	vec4 fragPosLightSpace = u_LightSpaceMatrix * vec4(gPos, 1.0f);
	
	// ambient
	vec3 ambient = gAmbient;
	
    // diffuse
    float diff = max(dot(gNormal, lightDir), 0.0);
	vec3 diffuse = gDiffuse * diff * u_LightDiffuse;
	
    // specular  
	vec3 reflectDir = reflect(lightDir, gNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	vec3 specular = gSpec * spec * u_LightSpecular;
    
	// shadow mapping
	float shadow = 0.0f;
	//shadow = ShadowCalculation(fragPosLightSpace, gNormal);
	shadow = ShadowIntensity(fragPosLightSpace);
	
	// combine results
    return (ambient) + (1.0 - shadow) * (diffuse + specular);
}

// calculates the color when using a directional light.
vec3 CalcSpotLight(vec3 gPos, vec3 gNormal, vec3 gAmbient, vec3 gDiffuse, vec3 gSpec, float gShininess)
{
    vec3 lightDir = normalize(u_LightPos - gPos);
	vec3 viewDir  = normalize(u_ViewPos - gPos);
	vec4 fragPosLightSpace = u_LightSpaceMatrix * vec4(gPos, 1.0f);
	float dist = length(u_LightPos - gPos);
	float attenuation = 1.0 / (u_LightAttenuation.x + u_LightAttenuation.y * dist + u_LightAttenuation.z * dist * dist);
	
	// Ambient
	 vec3 ambient = gAmbient;
	 
    // diffuse
    float diff = max(dot(gNormal, lightDir), 0.0);
	vec3 diffuse = diff * gDiffuse * u_LightDiffuse;
	
    // specular
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(gNormal, halfwayDir), 0.0), 64.0);
    vec3 specular = vec3(spec);
	
	// spotlight cut-off
	float theta = dot(lightDir, normalize(u_LightDir));
	float epsilon = u_LightCutOff - u_LightOuterCutOff;
	float intensity = clamp((theta - u_LightOuterCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;
	
	// shadow mapping
	float shadow = 0.0f;
	//shadow = ShadowCalculation(fragPosLightSpace, gNormal);
	shadow = ShadowIntensity(fragPosLightSpace);
	
    // combine results
    return (ambient) + (1.0 - shadow) * (diffuse + specular);
}

void main()
{             
    // retrieve data from gbuffer
	vec3 pos = vec3( texture( g_Position, v_TexCoord ) );
    vec3 worldNormal = vec3( texture( g_WorldNormal, v_TexCoord ) );
    vec3 ambient = texture(g_Ambient, v_TexCoord).rgb;
	vec3 diffuse = texture(g_Diffuse, v_TexCoord).rgb;
    vec3 specular = texture(g_Specular, v_TexCoord).rgb;
	
	vec3 result = CalcDirLight(pos, worldNormal, ambient, diffuse, specular, 1.0f);
    //vec3 result = CalcSpotLight(pos, worldNormal, ambient, diffuse, specular, 1.0f);
	
    color = vec4(result, 1.0f);
}
