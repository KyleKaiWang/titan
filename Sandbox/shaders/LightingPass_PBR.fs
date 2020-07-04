#version 450 core

const float PI = 3.141592;
const float Epsilon = 0.00001;

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D g_Position;
layout(binding = 1) uniform sampler2D g_WorldNormal;
layout(binding = 2) uniform sampler2D g_Albedo;
layout(binding = 3) uniform sampler2D g_Normal;
layout(binding = 4) uniform sampler2D g_MetallicRoughness;

uniform vec3 u_LightPos;
uniform vec3 u_LightDir;
uniform vec3 u_LightDiffuse;
uniform vec3 u_ViewPos;

// Shlick's approximation of the Fresnel factor.
vec3 FresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float DistributionGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float GeometricSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

void main()
{             
    // retrieve data from gbuffer
	vec3 pos = vec3( texture( g_Position, v_TexCoord ) );
    vec3 worldNormal = vec3( texture( g_WorldNormal, v_TexCoord ) );
    vec3 albedo = texture(g_Albedo, v_TexCoord).rgb;
    float specular = texture(g_Albedo, v_TexCoord).a;
	vec3 normal = texture(g_Normal, v_TexCoord).rgb;
	float metallic = texture(g_MetallicRoughness, v_TexCoord).r;
	float roughness = texture(g_MetallicRoughness, v_TexCoord).g;
	
	//vector from world-space fragment to the eye
	vec3 Lo = normalize(u_ViewPos - pos);
		
	// transform fragment's normal to world space
	//vec3 N = normalize(2.0 * texture(normal, v_TexCoord).rgb - 1.0);
	//N = normalize(v_TangentBasis * N);
	vec3 N = worldNormal;
	
	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(N, Lo));
		
	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * N - Lo;

	// Fresnel reflectance at normal incidence (for metals use albedo color).
	vec3 F0 = mix(Fdielectric, albedo, metallic);
	
	vec3 directLighting = vec3(0);
	{	
		//Using Light Pos to calculate direction
		vec3 Li = normalize(u_LightPos - pos);
		
		vec3 Lradiance  = u_LightDiffuse;
		vec3 Lh = normalize(Li + Lo);
		
		float cosLi = max(0.0, dot(N, Li));
		float cosLh = max(0.0, dot(N, Lh));
		
		//Fresnel term
		vec3 F = FresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		
		//Normal distribution for specular BRDF
		float D = DistributionGGX(cosLh, roughness);
		
		//Geometric attenuation for specular BRDF
		float G = GeometricSchlickGGX(cosLi, cosLo, roughness);
		
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);
		
		//Lambert diffuse BRDF
		vec3 diffuseBRDF = kd * albedo;
		
		//Cook-Torrance specular microfacet BRDF
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);
		
		directLighting = (diffuseBRDF + specularBRDF) * Lradiance  * cosLi;
	}
	
	vec3 ambientLighting = vec3(0);
	{
		//TODO:
		ambientLighting = vec3(0.03) * albedo;
		//vec3 irradiance = texture(
	}
	
	vec3 result = directLighting + ambientLighting;
    
    color = vec4(result, 1.0f);
	//color = vec4(worldNormal, 1.0f);
}
