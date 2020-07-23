#version 450 core

const float PI = 3.141592;
const float Epsilon = 0.00001;
const int NumLights = 4;

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D g_Position;
layout(binding = 1) uniform sampler2D g_WorldNormal;
layout(binding = 2) uniform sampler2D g_Albedo;
layout(binding = 3) uniform sampler2D g_Normal;
layout(binding = 4) uniform sampler2D g_MetallicRoughness;
layout(binding = 5) uniform samplerCube g_SpecularTexture;
layout(binding = 6) uniform samplerCube g_IrradianceTexture;
layout(binding = 7) uniform sampler2D g_SpecularBRDF_LUT;
layout(binding = 8) uniform sampler2D g_SSAO;

uniform vec3 u_LightPos[NumLights];
uniform vec3 u_LightDir;
uniform vec3 u_LightDiffuse;
uniform vec3 u_ViewPos;
uniform int u_EnableSSAO = 1;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

void main()
{             
    // retrieve data from gbuffer
	vec3 pos = vec3( texture( g_Position, v_TexCoord ) );
    vec3 worldNormal = vec3( texture( g_WorldNormal, v_TexCoord ) );
    vec3 albedo = texture(g_Albedo, v_TexCoord).rgb;
	//vec3 normal = texture(g_Normal, v_TexCoord).rgb;
	float ao = 0.0;
	if(u_EnableSSAO == 1)
		ao = min(1.0f, texture(g_SSAO, v_TexCoord).r);
	else
		ao = 1.0f;
	vec3 tangentNormal = texture(g_Normal, v_TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(pos);
    vec3 Q2  = dFdy(pos);
    vec2 st1 = dFdx(v_TexCoord);
    vec2 st2 = dFdy(v_TexCoord);

    vec3 N  = normalize(worldNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
	vec3 normal = normalize(TBN * tangentNormal);
	
	float metallic = texture(g_MetallicRoughness, v_TexCoord).r;
    float roughness = texture(g_MetallicRoughness, v_TexCoord).g;
	
	// input lighting data
    N = normal;
    vec3 V = normalize(u_ViewPos - pos);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(u_LightPos[i] - pos);
        vec3 H = normalize(V + L);
        float distance = length(u_LightPos[i] - pos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = u_LightDiffuse * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);    
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	                
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(g_IrradianceTexture, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 10.0;
    vec3 prefilteredColor = textureLod(g_SpecularTexture, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(g_SpecularBRDF_LUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
	vec3 ambient = (kD * diffuse + specular);
    vec3 col = (ambient + Lo) * ao;

    // HDR tonemapping
    col = col / (col + vec3(1.0));
	
    // gamma correct
    col = pow(col, vec3(1.0/2.2)); 
	
    color = vec4(col , 1.0f);
}
