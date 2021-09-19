#version 450
#pragma name SMAA_Pass1
//-----------------------------------------------------------------------------
// Blending Weight Calculation Shader (Second Pass)

layout(push_constant) uniform Push {
    vec4 SourceSize;
    vec4 OriginalSize;
    vec4 OutputSize;
    uint FrameCount;
} params;

layout(std140, set = 0, binding = 0) uniform UBO {
    mat4 MVP;
} global;

#define SMAA_RT_METRICS vec4(params.SourceSize.z, params.SourceSize.w, params.SourceSize.x, params.SourceSize.y)
#define SMAA_GLSL_4
#define SMAA_INCLUDE_PS 0
#include "SMAA.hlsl"

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec2 pixcoord;
layout(location = 2) out vec4 offset[3];

void main() {
    gl_Position = global.MVP * Position;
    vTexCoord = TexCoord;
    SMAABlendingWeightCalculationVS(TexCoord, pixcoord, offset);
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec2 pixcoord;
layout(location = 2) in vec4 offset[3];
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D Source;
layout(set = 0, binding = 3) uniform sampler2D areaTex;
layout(set = 0, binding = 4) uniform sampler2D searchTex;

#include "SMAA.frag"

void main() {
    vec4 subsampleIndices = vec4(0.0);
    FragColor = SMAABlendingWeightCalculationPS(vTexCoord, pixcoord, offset, Source, areaTex, searchTex, subsampleIndices);
}
