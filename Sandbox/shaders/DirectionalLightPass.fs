#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D g_PositionTexture;
uniform sampler2D g_NormalTexture;
uniform sampler2D g_DiffuseTexture;

struct DirLight {
    vec3 Direction;
};

uniform DirLight u_DirLight;
uniform vec3 u_ViewPos;

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 gPos, vec3 gNormal, vec3 gColor, float gSpec)
{
    vec3 lightDir = normalize(-light.Direction);
	vec3 viewDir  = normalize(u_ViewPos - gPos);
	
    // diffuse
    float diff = max(dot(gNormal, lightDir), 0.0);
	
    // specular
    vec3 reflectDir = reflect(-lightDir, gNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
	
    // combine results
    vec3 ambient = gColor;
    vec3 diffuse = diff * gColor;
    vec3 specular = spec * vec3(gSpec);
    return (ambient + diffuse);
	
	//Only diffuse version
    //vec3 lightDir = normalize(-light.Direction);
    //vec3 diffuse = max(dot(gNormal, lightDir), 0.0) * gColor;
    //return diffuse;
}

void main()
{             
    // retrieve data from gbuffer
	vec3 pos = vec3( texture( g_PositionTexture, v_TexCoord ) );
    vec3 normal = vec3( texture( g_NormalTexture, v_TexCoord ) );
    vec3 diff = vec3( texture(g_DiffuseTexture, v_TexCoord) );
    float specular = texture(g_DiffuseTexture, v_TexCoord).a;
	
	vec3 result = CalcDirLight(u_DirLight, pos, normal, diff, specular);
    
    color = vec4(result, 1.0f);
}
