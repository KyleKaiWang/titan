#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

layout(binding = 1) uniform sampler2D g_PositionTexture;
layout(binding = 2) uniform sampler2D g_NormalTexture;
layout(binding = 3) uniform sampler2D g_DiffuseTexture;
layout(binding = 4) uniform sampler2D g_ShadowMap;

uniform vec3 u_LightPos;
uniform vec3 u_LightDir;
uniform vec3 u_ViewPos;
uniform mat4 u_LightSpaceMatrix;

float ShadowCalculation(vec4 fragPosLightSpace)
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
	
	//------------------------------------------------------------------------
	//vec2 shadowIndex = fragPosLightSpace.xy / fragPosLightSpace.w;
	//float lightDepth = texture(g_ShadowMap, shadowIndex).w;
	//float pixelDepth = fragPosLightSpace.w;
	//shadow = pixelDepth > lightDepth ? 1.0 : 0.0;

	//vec4 shadowCoord = fragPosLightSpace;
	//shadowCoord /= shadowCoord.w;
	//
	//float shadow = 1.0;
    //if(texture(g_ShadowMap, shadowCoord.xy).z < shadowCoord.z - 0.005 ) 
	//	shadow = 0.5f;

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
	vec3 normal = vec3( texture( g_NormalTexture, v_TexCoord ) );
    vec3 lightDir = normalize(u_LightPos - gPos);
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
vec3 CalcDirLight(vec3 gPos, vec3 gNormal, vec3 gColor, float gSpec)
{
    vec3 lightDir = normalize(u_LightPos - gPos);
	vec3 viewDir  = normalize(u_ViewPos - gPos);
	vec4 fragPosLightSpace = u_LightSpaceMatrix * vec4(gPos, 1.0f);
	
    // diffuse
    float diff = max(dot(gNormal, lightDir), 0.0);
	
    // specular
    //vec3 reflectDir = reflect(-lightDir, gNormal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
	
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(gNormal, halfwayDir), 0.0), 16.0);
	
    // combine results
	vec3 lightColor = vec3(0.3);
    vec3 ambient = gColor * lightColor;
    vec3 diffuse = diff * gColor;
    vec3 specular = vec3(spec) * lightColor;
	float shadow = ShadowCalculationPCF(fragPosLightSpace, gPos); 
	//float shadow = ShadowCalculation(fragPosLightSpace); 
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

void main()
{             
    // retrieve data from gbuffer
	vec3 pos = vec3( texture( g_PositionTexture, v_TexCoord ) );
    vec3 normal = vec3( texture( g_NormalTexture, v_TexCoord ) );
    vec3 diff = vec3( texture(g_DiffuseTexture, v_TexCoord) );
    float specular = texture(g_DiffuseTexture, v_TexCoord).a;
	
	vec3 result = CalcDirLight(pos, normal, diff, specular);
    
    color = vec4(result, 1.0f);
}
