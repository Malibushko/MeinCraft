#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D u_ScreenTexture;

void main()
{
	vec3 Color = texture(u_ScreenTexture, TexCoords).rgb;

	float Brightness = dot(Color, vec3(0.2126, 0.7152, 0.0722));

    if (Brightness > 1.0)
        FragColor = vec4(Color, 1.0);
    else
        FragColor= vec4(0.0, 0.0, 0.0, 1.0);
}