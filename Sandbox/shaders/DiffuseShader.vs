#Version 400

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

out vec3 v_LightIntensity;

uniform vec4 u_LightPos;
uniform vec3 u_Kd;
uniform vec3 u_Ld;

uniform mat4 u_ModelViewMatrix;
uniform mat3 u_NormalMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_MVP;

void main()
{
    vec3 tnorm = normalize(u_NormalMatrix * a_Normal);
    vec4 camCoords = u_ModelViewMatrix * vec4(a_Position, 1.0f);
    vec3 s = normalize(vec3(u_LightPos - camCoords));

    v_LightIntensity = u_Kd * u_Ld * max( dot(s, tnorm), 0.0);
    gl_Position = MVP * vec4(a_Position, 1.0f);
}