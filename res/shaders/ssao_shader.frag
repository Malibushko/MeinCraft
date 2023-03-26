#version 460 core
#define TILE_SIZE 4

layout(std430, binding = 0) readonly buffer MatricesBuffer
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
  mat4 InverseMVP;
};

out float FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gNoiseTexture;

layout(location = 0) uniform vec3 Samples[64];

const int   KernelSize = 64;
const float Radius     = 0.5;
const float Bias       = 0.025;

void main()
{
    vec2       ScreenSize = textureSize(gPosition, 0);
    const vec2 NoiseScale = vec2(ScreenSize.x / TILE_SIZE, ScreenSize.y / TILE_SIZE);

    // get input for SSAO algorithm
    vec3 FragPos   = texture(gPosition, TexCoords).xyz;
    vec3 Normal    = normalize(texture(gNormal, TexCoords).rgb);
    vec3 RandomVec = normalize(texture(gNoiseTexture, TexCoords * NoiseScale).xyz);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 Tangent   = normalize(RandomVec - Normal * dot(RandomVec, Normal));
    vec3 Bitangent = cross(Normal, Tangent);
    mat3 TBN       = mat3(Tangent, Bitangent, Normal);

    // iterate over the sample kernel and calculate occlusion factor
    float Occlusion = 0.0;

    for(int i = 0; i < KernelSize; ++i)
    {
        vec3 SamplePos = TBN * Samples[i];

        SamplePos = FragPos + SamplePos * Radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 Offset = vec4(SamplePos, 1.0);
        Offset      = Projection * Offset; // from view to clip-space
        Offset.xyz /= Offset.w; // perspective divide
        Offset.xyz  = Offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get sample depth
        float SampleDepth = texture(gPosition, Offset.xy).z; // get depth value of kernel sample

        // range check & accumulate
        float RangeCheck = smoothstep(0.0, 1.0, Radius / abs(FragPos.z - SampleDepth));

        Occlusion += (SampleDepth >= SamplePos.z + Bias ? 1.0 : 0.0) * RangeCheck;
    }

    Occlusion = 1.0 - (Occlusion / KernelSize);

    FragColor = Occlusion;
}