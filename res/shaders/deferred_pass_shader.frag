#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TextureCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D u_Texture_0;

void main()
{
  gPosition       = FragPos;
  gNormal         = normalize(Normal);
  gAlbedoSpec.rgb = texture(u_Texture_0, TextureCoords).rgb;
  gAlbedoSpec.a   = 0.0;
}