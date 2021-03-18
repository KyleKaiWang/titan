#version 450 core

const int MAX_POINT_LIGHTS = 4;

struct DirectionalLight
{
	mat4 space_matrix;
	vec3 direction;
	float dummy;
};

struct SpotLight
{
	vec4 position;  // Position in eye coords.
	vec3 intensity; // Amb., Diff., and Specular intensity
	vec3 direction; // Normalized direction of the spotlight 
	float exponent; // Angular attenuation exponent
	float cutoff;   // Cutoff angle (between 0 and 90)
};

struct PointLight
{
	vec3 position;
	float radius;
	vec3 color;
};

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D g_Position;
layout(binding = 1) uniform sampler2D g_WorldNormal;
layout(binding = 2) uniform sampler2D g_Ambient;
layout(binding = 3) uniform sampler2D g_Diffuse;
layout(binding = 4) uniform sampler2D g_Specular;
layout(binding = 5) uniform sampler2D g_ShadowMap;

// Lighting
uniform vec3 u_ViewPos;
DirectionalLight dirLight;
PointLight pointLights[MAX_POINT_LIGHTS];

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(g_ShadowMap, projCoords.xy).r;
	
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}

float ShadowCalculationPCF(vec4 fragPosLightSpace, vec3 gPos)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(g_ShadowMap, projCoords.xy).r; 
	
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	
    // calculate bias (based on depth map resolution and slope)
	vec3 normal = vec3( texture( g_WorldNormal, v_TexCoord ) );
    vec3 lightDir = normalize(-dirLight.direction);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(g_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(g_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 CalcLighting(vec3 gPos, vec3 gNormal, vec3 gDiffuse, vec3 gSpec, float gShininess) 
{ 
	// Calculate the diffuse term 
	vec3 L = vec3(0.0f); 
	float attenuation = 1.0f; 
	
	for(int i = 0; i < MAX_POINT_LIGHTS; ++i) {
		// Base the the light vector on the light position 
		L = pointLights[i].position - gPos; 
		
		// Calculate attenuation based on distance from the light source 
		float dist = length( L ); 
		attenuation = max( 0, 1.0f - ( dist / pointLights[i].radius ) ); 
		L /= dist; 
	}
	
	// Light direction is explicit for directional lights 
	L = -dirLight.direction; 
	
	//#if SPOTLIGHT 
	//// Also add in the spotlight attenuation factor 
	//float3 L2 = LightDirection; 
	//float rho = dot( -L, L2 ); 
	//attenuation *= clamp( ( rho - SpotlightAngles.y )  / ( SpotlightAngles.x - SpotlightAngles.y ), 0.0f, 1.0f ); 
	//#endif 
	
	float nDotL = clamp( dot( gNormal, L ), 0.0f, 1.0f ); 
	vec3 diffuse = nDotL * gDiffuse; 
	
	// Calculate the specular term 
	vec3 V = u_ViewPos - gPos; 
	vec3 H = normalize( L + V ); 
	vec3 specular = pow( clamp( dot( gNormal, H ), 0.0f, 1.0f ), gShininess ) * gSpec * nDotL; 
	
	// Final value is the sum of the albedo and diffuse with attenuation applied 
	return ( diffuse + specular ) * attenuation; 
} 

void main()
{             
    // retrieve data from gbuffer
	vec3 pos = texture( g_Position, v_TexCoord ).rgb;
    vec3 worldNormal = texture( g_WorldNormal, v_TexCoord ).rgb;
	//vec3 ambient = texture(g_Ambient, v_TexCoord).rgb;
	vec3 diffuse = texture(g_Diffuse, v_TexCoord).rgb;
	vec3 specular = texture(g_Specular, v_TexCoord).rgb;
	float shininess = texture(g_Specular, v_TexCoord).a;
	
	vec3 result = CalcLighting(pos, worldNormal, diffuse, specular, 1.0f);
    
	result = pow(result, vec3(1.0/2.2));
    color = vec4(result, 1.0f);
}
