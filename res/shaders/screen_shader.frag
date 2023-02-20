#version 460 core

// shader inputs
in vec2 TextureCoords;

// shader outputs
layout (location = 0) out vec4 FragmentColor;

// screen image
layout(binding = 0) uniform sampler2D Screen;

void main()
{
	vec4 Color = texture(Screen, TextureCoords);

	if (Color.a < 0.1)
		discard;

	FragmentColor = Color;
}