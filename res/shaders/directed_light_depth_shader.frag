#version 460 core

layout(binding=0) uniform sampler2D u_Texture_0;

in vec2 TextureCoords;

void main()
{
	vec4 Color = texture(u_Texture_0, TextureCoords);

	if (Color.a < 0.1)
		discard;
}