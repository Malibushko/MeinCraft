#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D u_ScreenTexture;
layout(binding = 1) uniform sampler2D u_SSAOTexture;

const float SSAO_STRENGTH = 0.5;

void main()
{
	vec3  Color      = texture(u_ScreenTexture, TexCoords).rgb;
	float SSAOFactor = texture(u_SSAOTexture, TexCoords).r;

	vec3 FinalColor = Color * clamp(SSAOFactor - SSAO_STRENGTH, 0, 1);

	FragColor = vec4(FinalColor, 1.0);
}