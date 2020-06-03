#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D g_PositionTexture;
uniform sampler2D g_NormalTexture;
uniform sampler2D g_DiffuseTexture;

//uniform vec4 u_LightPosition;
//uniform float u_LightIntensity;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float Radius;
};
const int NR_LIGHTS = 32;
uniform Light u_Lights[NR_LIGHTS];
uniform vec3 u_ViewPos;

void main()
{             
    // retrieve data from gbuffer
	vec3 pos = vec3( texture( g_PositionTexture, v_TexCoord ) );
    vec3 normal = vec3( texture( g_NormalTexture, v_TexCoord ) );
    vec3 diffuseColor = vec3( texture(g_DiffuseTexture, v_TexCoord) );
    float Specular = texture(g_DiffuseTexture, v_TexCoord).a;
    
    // then calculate lighting as usual
    vec3 lighting  = diffuseColor * 0.5; // hard-coded ambient component
    vec3 viewDir  = normalize(u_ViewPos - pos);
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // calculate distance between light source and current fragment
        float distance = length(u_Lights[i].Position - pos);
        if(distance < u_Lights[i].Radius)
        {
            // diffuse
            vec3 lightDir = normalize(u_Lights[i].Position - pos);
            vec3 diffuse = max(dot(normal, lightDir), 0.0) * diffuseColor * u_Lights[i].Color;
            
			// specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
            vec3 specular = u_Lights[i].Color * spec * Specular;
			
            // attenuation
            float attenuation = 1.0 / (1.0 + u_Lights[i].Linear * distance + u_Lights[i].Quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }    
    color = vec4(lighting, 1.0);
}
