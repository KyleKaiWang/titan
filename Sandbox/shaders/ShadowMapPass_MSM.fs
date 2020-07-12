#version 450 core

layout(location = 0) out vec4 g_ShadowMap;

void main()
{
	g_ShadowMap.r = gl_FragCoord.z;
	g_ShadowMap.g = gl_FragCoord.z * gl_FragCoord.z;
	g_ShadowMap.b = gl_FragCoord.z * gl_FragCoord.z * gl_FragCoord.z;
	g_ShadowMap.a = gl_FragCoord.z * gl_FragCoord.z * gl_FragCoord.z * gl_FragCoord.z;
	
	//Debug
	//g_ShadowMap = vec4(vec3(gl_FragCoord.z),1.0f);
}