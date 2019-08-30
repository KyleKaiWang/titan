layout (location = 0) out vec4 color;

in vec3 v_LightIntensity;

void main()
{
    color = vec4(v_LightIntensity, 1.0f);
}