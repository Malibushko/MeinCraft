#version 460 core
out float FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D u_TextureSSAO;

void main()
{
    const vec2 TexelSize = 1.0 / vec2(textureSize(u_TextureSSAO, 0));

    float Result = 0.0;

    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 Offset = vec2(float(x), float(y)) * TexelSize;

            Result += texture(u_TextureSSAO, TexCoords + Offset).r;
        }
    }

    FragColor = Result / (4.0 * 4.0);
}