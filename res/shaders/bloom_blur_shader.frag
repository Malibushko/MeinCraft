#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D u_BloomTexture;

layout(location = 0) uniform bool IsHorizontal;

uniform float Weights[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
     vec2 TextureOffset = 1.0 / textureSize(u_BloomTexture, 0);
     vec3 Result        = texture(u_BloomTexture, TexCoords).rgb * Weights[0];

     if(IsHorizontal)
     {
         for (int i = 1; i < 5; ++i)
         {
            Result += texture(u_BloomTexture, TexCoords + vec2(TextureOffset.x * i, 0.0)).rgb * Weights[i];
            Result += texture(u_BloomTexture, TexCoords - vec2(TextureOffset.x * i, 0.0)).rgb * Weights[i];
         }
     }
     else
     {
         for (int i = 1; i < 5; ++i)
         {
             Result += texture(u_BloomTexture, TexCoords + vec2(0.0, TextureOffset.y * i)).rgb * Weights[i];
             Result += texture(u_BloomTexture, TexCoords - vec2(0.0, TextureOffset.y * i)).rgb * Weights[i];
         }
     }

     FragColor = vec4(Result, 1.0);
}