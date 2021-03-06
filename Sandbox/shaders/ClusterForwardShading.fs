#version 450 core

const float M_PI = 3.141592;
const float Epsilon = 0.00001;
const int NumLights = 4;

const vec3 Fdielectric = vec3(0.04);

layout(location = 0) out vec4 color;

in vec3 v_FragPos_WS;
in vec2 v_TexCoord;
in vec4 v_FragPos_LS;
in vec3 v_T;
in vec3 v_B;
in vec3 v_N;

struct DirLight{
    vec3 direction;
    vec3 color;
};
uniform DirLight dirLight;

uniform sampler2D g_Albedo;
uniform sampler2D g_Normal;
uniform sampler2D g_Metallic;
uniform sampler2D g_Roughness;
uniform sampler2D g_ShadowMap;

uniform samplerCube g_PrefilterMap;
uniform samplerCube g_IrradianceMap;
uniform sampler2D g_SpecularBRDF_LUT;
uniform sampler2D g_LightMap;

struct PointLight{
    vec4 position;
    vec4 color;
    bool enabled;
    float intensity;
    float range;
};

struct LightGrid{
    uint offset;
    uint count;
};

layout (std430, binding = 2) buffer screenToView{
    mat4 inverseProjection;
    uvec4 tileSizes;
    uvec2 screenDimensions;
    float scale;
    float bias;
};
layout (std430, binding = 3) buffer lightSSBO{
    PointLight pointLight[];
};

layout (std430, binding = 4) buffer lightIndexSSBO{
    uint globalLightIndexList[];
};

layout (std430, binding = 5) buffer lightGridSSBO{
    LightGrid lightGrid[];
};

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

vec3 debugDepthColors[8] = vec3[](
   vec3(0, 0, 0),  vec3( 0,  0,  1), vec3( 0, 1, 0),  vec3(0, 1, 1),
   vec3(1, 0, 0),  vec3( 1,  0,  1), vec3( 1, 1, 0),  vec3(1, 1, 1)
);

uniform samplerCube depthMaps[NumLights];
uniform float u_FarPlane;
uniform float zFar;
uniform float zNear;
uniform bool b_IBL;
uniform bool b_Slices;
uniform vec3 u_CameraPos;

vec3 turboColormap(float x)
{
    // show clipping
    if(x < 0.0)
        return vec3(0.0);
    else if(x > 1.0)
        return vec3(1.0);

    const vec4 kRedVec4   = vec4(0.13572138, 4.61539260, -42.66032258, 132.13108234);
    const vec4 kGreenVec4 = vec4(0.09140261, 2.19418839, 4.84296658, -14.18503333);
    const vec4 kBlueVec4  = vec4(0.10667330, 12.64194608, -60.58204836, 110.36276771);
    const vec2 kRedVec2   = vec2(-152.94239396, 59.28637943);
    const vec2 kGreenVec2 = vec2(4.27729857, 2.82956604);
    const vec2 kBlueVec2  = vec2(-89.90310912, 27.34824973);

    x = clamp(x, 0.0, 1.0);
    vec4 v4 = vec4(1.0, x, x * x, x * x * x);
    vec2 v2 = v4.zw * v4.z;
    return vec3(
        dot(v4, kRedVec4)   + dot(v2, kRedVec2),
        dot(v4, kGreenVec4) + dot(v2, kGreenVec2),
        dot(v4, kBlueVec4)  + dot(v2, kBlueVec2)
    );
}

float screen2EyeDepth(float depth, float near, float far)
{
	float ndc = 2.0 * depth - 1.0;
	float eye = 2.0 * far * near / (far + near + ndc * (far - near));
    return eye;
}

// cluster depth index from depth in screen coordinates (gl_FragCoord.z)
uint getClusterZIndex(float screenDepth)
{
    float scale = float(tileSizes.z) / log(zFar / zNear);
    float bias = -(float(tileSizes.z) * log(zNear) / log(zFar / zNear));
	scale = 0;
	bias = 0;
    float eyeDepth = screen2EyeDepth(screenDepth, zNear, zFar);
    uint zIndex = uint(max(log(eyeDepth) * scale + bias, 0.0));
    return zIndex;
}

// cluster index from fragment position in window coordinates (gl_FragCoord)
uint getClusterIndex(vec4 fragCoord)
{
    uint zIndex = getClusterZIndex(fragCoord.z);
    uvec3 indices = uvec3(uvec2(fragCoord.xy / tileSizes.xy), zIndex);
    uint cluster = (tileSizes.x * tileSizes.y) * indices.z +
                   tileSizes.x * indices.y +
                   indices.x;
    return cluster;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;

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
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float CalcPointLightShadows(samplerCube depthMap, vec3 fragToLight, float viewDistance){
    float shadow      = 0.0;
    float bias        = 0.0;
    int   samples     = 8;
    float fraction    = 1.0/float(samples);
    float diskRadius  = (1.0 + (viewDistance / u_FarPlane)) / 25.0;
    float currentDepth = (length(fragToLight) - bias);

    for(int i = 0; i < samples; ++i){
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i], diskRadius).r;
        closestDepth *= u_FarPlane;
        if(currentDepth > closestDepth){
            shadow += fraction;
        }
    }
    return shadow;
}

vec3 CalcPointLight(uint index, vec3 normal, vec3 fragPos,
                    vec3 viewDir, vec3 albedo, float rough,
                    float metal, vec3 F0,  float viewDistance){
    //Point light basics
    vec3 position = pointLight[index].position.xyz;
    vec3 color    = 100.0 * pointLight[index].color.rgb;
    float radius  = pointLight[index].range;

    //Stuff common to the BRDF subfunctions 
    vec3 lightDir = normalize(position - fragPos);
    vec3 halfway  = normalize(lightDir + viewDir);
    float nDotV = max(dot(normal, viewDir), 0.0);
    float nDotL = max(dot(normal, lightDir), 0.0);

    //Attenuation calculation that is applied to all
    float distance    = length(position - fragPos);
    float attenuation = pow(clamp(1 - pow((distance / radius), 4.0), 0.0, 1.0), 2.0)/(1.0  + (distance * distance) );
    vec3 radianceIn   = color * attenuation;

    //Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, halfway, rough);
    float G   = GeometrySmith(normal, viewDir, lightDir, rough);
    vec3  F   = FresnelSchlick(max(dot(halfway,viewDir), 0.0), F0);

    //Finding specular and diffuse component
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metal;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * nDotV * nDotL;
    vec3 specular = numerator / max(denominator, 0.0000001);
    // vec3 specular = numerator / denominator;

    vec3 radiance = (kD * (albedo / M_PI) + specular ) * radianceIn * nDotL;

    //shadow stuff
    vec3 fragToLight = fragPos - position;
    //float shadow = CalcPointLightShadows(depthMaps[index], fragToLight, viewDistance);
    
    //radiance *= (1.0 - shadow);

    return radiance;
}

float CalcDirShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float bias = 0.0;
    int   samples = 9;
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(g_ShadowMap, 0);

    for(int i = 0; i < samples; ++i){
        float pcfDepth = texture(g_ShadowMap, projCoords.xy + sampleOffsetDirections[i].xy * texelSize).r;
        shadow += projCoords.z - bias > pcfDepth ? 0.111111 : 0.0;
    }

    return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float rough, float metal, float shadow, vec3 F0)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfway  = normalize(lightDir + viewDir);
    float nDotV = max(dot(normal, viewDir), 0.0);
    float nDotL = max(dot(normal, lightDir), 0.0);
    vec3 radianceIn = dirLight.color;

    //Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, halfway, rough);
    float G   = GeometrySmith(normal, viewDir, lightDir, rough);
    vec3  F   = FresnelSchlick(max(dot(halfway,viewDir), 0.0), F0);

    //Finding specular and diffuse component
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metal;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * nDotV * nDotL;
    vec3 specular = numerator / max (denominator, 0.0001);

    vec3 radiance = (kD * (albedo / M_PI) + specular ) * radianceIn * nDotL;
    radiance *= (1.0 - shadow);

    return radiance;
}

void main()
{             
	vec3 albedo = texture(g_Albedo, v_TexCoord).rgb;
	float alpha = texture(g_Albedo, v_TexCoord).a;
	
    vec3 normal = normalize(2.0 * texture(g_Normal, v_TexCoord).rgb - 1.0);
	float ao = texture(g_LightMap, v_TexCoord).r;
	float metallic = texture(g_Metallic, v_TexCoord).r;
    float roughness = texture(g_Roughness, v_TexCoord).r;
	mat3 TBN = mat3(v_T, v_B, v_N);
	vec3 N = normalize(TBN * normal);
	vec3 V = normalize(u_CameraPos - v_FragPos_WS);
	vec3 R = reflect(-V, N);

	vec3 F0 = Fdielectric; 
    F0 = mix(F0, albedo, metallic);

    uint zTile     = uint(max(log2(screen2EyeDepth(gl_FragCoord.z, zNear, zFar)) * scale + bias, 0));
    uvec3 tiles    = uvec3( uvec2( gl_FragCoord.xy / tileSizes[3] ), zTile);
    uint tileIndex = tiles.x +
                     tileSizes.x * tiles.y +
                     (tileSizes.x * tileSizes.y) * tiles.z;
	
	// Shadow
    float shadow = CalcDirShadow(v_FragPos_LS);
    float viewDistance = length(u_CameraPos - v_FragPos_WS);
	
	//Directional light 
    vec3 radianceOut = CalcDirLight(dirLight, N, V, albedo, roughness, metallic, shadow, F0) ;

    // TODO : Point lights 
    //uint lightCount       = lightGrid[tileIndex].count;
    //uint lightIndexOffset = lightGrid[tileIndex].offset;
	
    //for(uint i = 0; i < lightCount; i++){
    //    uint lightVectorIndex = globalLightIndexList[lightIndexOffset + i];
    //    radianceOut += CalcPointLight(lightVectorIndex, N, v_FragPos_WS, V, albedo, roughness, metallic, F0, viewDistance);
    //}
	
	vec3 ambient = vec3(0.025)* albedo;
    if(b_IBL){
		vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
		vec3 kS = F;
		vec3 kD = 1.0 - kS;
		kD *= 1.0 - metallic;	  
		
		vec3 irradiance = texture(g_IrradianceMap, N).rgb;
		vec3 diffuse = irradiance * albedo;
		
		const float MAX_REFLECTION_LOD = 4.0;
		vec3 prefilteredColor = textureLod(g_PrefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
		vec2 brdf  = texture(g_SpecularBRDF_LUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
		vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
		
		ambient = (kD * diffuse + specular);
	}
	radianceOut += ambient;
	
	// Debugging lighting overhead 
	//if(b_debug_cluster_light){
	//	uint cluster = getClusterIndex(gl_FragCoord);
	//	LightGrid grid = lightGrids[cluster];
	//	int lights = int(grid.count);
	//	
	//	// show possible clipping
	//	if(lights == 0)
	//		lights--;
	//	else if(lights < MAX_LIGHTS_PER_CLUSTER)
	//		lights++;
	//
	//	vec3 lightCountColor = turboColormap(float(lights) / MAX_LIGHTS_PER_CLUSTER);
	//	FragColor = vec4(lightCountColor, 1.0);
    //}
	
	if(b_Slices){
        color = vec4(debugDepthColors[uint(zTile % 8)], 1.0);
    }
    else{
        color = vec4(radianceOut, 1.0);
    }
	//color = vec4(vec3(scale), 1.0);
}