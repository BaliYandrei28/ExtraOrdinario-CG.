#version 330 core
in vec3 Normal;
out vec4 FragColor;

uniform vec3 colorBronce;

void main()
{
    vec3 N = normalize(Normal);
    vec3 luz = normalize(vec3(-0.4, 1.0, 0.6));
    float difuso = max(dot(N, luz), 0.0);
    float intensidad = 0.60 + 0.40 * difuso;
    FragColor = vec4(colorBronce * intensidad, 1.0);
}
