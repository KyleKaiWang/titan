#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D g_Position;
layout(binding = 1) uniform sampler2D g_WorldNormal;

// Phong
layout(binding = 2) uniform sampler2D g_Ambient;
layout(binding = 3) uniform sampler2D g_Diffuse;
layout(binding = 4) uniform sampler2D g_Specular;

layout(binding = 5) uniform sampler2D g_ShadowMap;

// Lighting
uniform vec3 u_ViewPos;
uniform mat4 u_LightSpaceMatrix;
uniform vec3 u_LightPos;
uniform vec3 u_LightAmbient;
uniform vec3 u_LightDiffuse;
uniform vec3 u_LightSpecular;
uniform vec3 u_LightDir;

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
    vec3 lightDir = normalize(-u_LightDir);
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
	//vec3 halfwayDir = normalize(-lightDir + viewDir);  
	vec3 reflectDir = reflect(-lightDir, gNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
	vec3 specular = gSpec * spec * u_LightSpecular;
    
	// combine results
	float shadow = 0;
	shadow = ShadowCalculationPCF(fragPosLightSpace, gPos); 
	//shadow = ShadowCalculation(fragPosLightSpace, gNormal, lightDir);
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

void main()
{             
    // retrieve data from gbuffer
	vec3 pos = texture( g_Position, v_TexCoord ).rgb;
    vec3 worldNormal = texture( g_WorldNormal, v_TexCoord ).rgb;
	vec3 ambient = texture(g_Ambient, v_TexCoord).rgb;
	vec3 diffuse = texture(g_Diffuse, v_TexCoord).rgb;
	vec3 specular = texture(g_Specular, v_TexCoord).rgb;
	float shininess = texture(g_Specular, v_TexCoord).a;
	
	vec3 result = CalcDirLight(pos, worldNormal, ambient, diffuse, specular, 1.0f);
    
    color = vec4(result, 1.0f);
}
