#version 460 core

// shader inputs
in vec2 TextureCoords;

// shader outputs
layout (location = 0) out vec4 FragmentColor;

// screen image
uniform sampler2D Screen;

void main()
{
	FragmentColor = vec4(texture(Screen, TextureCoords).rgb, 1.0f);
}