#version 400 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

out vec3 v_LightIntensity;;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;

uniform vec4 u_LightPosition;
uniform vec3 La;
uniform vec3 Ld;
uniform vec3 Ls;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float u_Shininess;

void main()
{
	vec3 n = normalize(u_NormalMatrix * a_Normal);
	vec4 camCoords = u_ViewMatrix * u_Model * vec4(a_Position,1.0);
    
	vec3 ambient = La * Ka;
	vec3 s = normalize(vec3(u_LightPosition - camCoords));
	float sDotN = max( dot(s,n), 0.0 );
	vec3 diffuse = Ld * Kd * sDotN;
	vec3 specular = vec3(0.0);
	if( sDotN > 0.0 ) 
	{
	  vec3 v = normalize(-camCoords.xyz);
	  vec3 r = reflect( -s, n );
	  specular = Ls * Ks * pow( max( dot(r,v), 0.0 ), u_Shininess );
	}

	v_LightIntensity = ambient + diffuse + specular;
	
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_Model * vec4(a_Position, 1.0f);
}