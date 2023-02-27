#version 460 core
in vec2 TextureCoord;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_Texture_0;

void main()
{
	vec4 Color = texture(u_Texture_0, TextureCoord);

	if (Color.a < 0.1)
		discard;

	FragColor = Color;
}