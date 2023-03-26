#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D u_ScreenTexture;
layout(binding = 1) uniform sampler2D u_SSAOTexture;
layout(binding = 2) uniform sampler2D u_BloomTexture;

const float SSAO_STRENGTH = 0.5;

void main()
{
	const float Gamma = 2.2;

	vec3  Color      = texture(u_ScreenTexture, TexCoords).rgb;
	float SSAOFactor = texture(u_SSAOTexture, TexCoords).r;

	vec3 FinalColor = Color * clamp(SSAOFactor - SSAO_STRENGTH, 0, 1);

	FinalColor += texture(u_BloomTexture, TexCoords).rgb;

	// Reinhard tone mapping
	FinalColor = FinalColor / (FinalColor + vec3(1.0));

	FragColor = vec4(FinalColor, 1.0);
}