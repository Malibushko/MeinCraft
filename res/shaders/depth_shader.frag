#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;

layout(binding=0) uniform sampler2D u_Texture_0;

in vec3 Position;
in vec2 TextureCoords;
in vec3 Normal;

void main()
{
	vec4 Color = texture(u_Texture_0, TextureCoords);

	if (Color.a < 0.1)
		discard;

	gPosition = Position;
	gNormal   = normalize(Normal);
}