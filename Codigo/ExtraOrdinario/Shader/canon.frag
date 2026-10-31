#version 330 core
in vec2 TexCoords;
in vec3 Normal;
out vec4 FragColor;

uniform sampler2D canonAtlas;

void main()
{
    vec3 colorOriginal = texture(canonAtlas, TexCoords).rgb;
    vec3 N = normalize(Normal);
    vec3 luz = normalize(vec3(-0.4, 1.0, 0.6));
    float difuso = max(dot(N, luz), 0.0);
    float iluminacion = 0.65 + 0.35 * difuso;
    FragColor = vec4(colorOriginal * iluminacion, 1.0);
}
