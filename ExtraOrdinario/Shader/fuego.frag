#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

// Mesh::Draw vincula la textura difusa del OBJ en la unidad 0.
uniform sampler2D texture_diffuse1;

void main()
{
    vec4 tex = texture(texture_diffuse1, TexCoord);
    float brillo = max(tex.r, max(tex.g, tex.b));

    // Evitar el fondo negro de la imagen aunque el cargador pierda el alfa.
    if (brillo < 0.035 || tex.a < 0.01)
        discard;

    float opacidad = tex.a * smoothstep(0.025, 0.32, brillo);
    FragColor = vec4(min(tex.rgb * 1.55, vec3(1.0)), opacidad);
}
