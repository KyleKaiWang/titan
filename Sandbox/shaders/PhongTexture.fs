#version 400 core

layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec4 u_LightPosition;
uniform vec3 u_LightIntensity;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float u_Shininess;

uniform vec4 u_Color;

void Phong(vec3 pos, vec3 normal, out vec3 amb, out vec3 specular)
{
	vec3 s = normalize(u_LightPosition.xyz - pos); 
	vec3 v = normalize(-pos.xyz);
	vec3 r = reflect( -s, normal );
	vec3 ambient = u_LightIntensity * Ka;
	float sDotN = max( dot(s,normal), 0.0 );
	vec3 diffuse = u_LightIntensity * Kd * sDotN;
	specular = vec3(0.0);
	if( sDotN > 0.0 ) 
	{
	  specular = u_LightIntensity * Ks * pow( max( dot(r,v), 0.0 ), u_Shininess );
	}
	amb = ambient + diffuse;
}

void main()
{
	vec3 amb, specular;
	vec4 texColor = texture( u_Texture, v_TexCoord );
	Phong(v_Position, v_Normal, amb, specular);
	color =  vec4(amb, 1.0f) * texColor + vec4(specular, 1.0f);
}