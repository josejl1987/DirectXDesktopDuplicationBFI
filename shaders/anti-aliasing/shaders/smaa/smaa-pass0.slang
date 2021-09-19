#version 450
#pragma name SMAA_Pass0
//-----------------------------------------------------------------------------
// Edge Detection Shaders (First Pass)

#pragma parameter SMAA_EDT "SMAA Edge Detection: Luma | Color" 0.0 0.0 1.0 1.0

layout(push_constant) uniform Push {
    vec4 SourceSize;
    vec4 OriginalSize;
    vec4 OutputSize;
    uint FrameCount;
    float SMAA_EDT;
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
layout(location = 1) out vec4 offset[3];

void main() {
    gl_Position = global.MVP * Position;
    vTexCoord = TexCoord;
    SMAAEdgeDetectionVS(TexCoord, offset);
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec4 offset[3];
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D Source;

#include "SMAA.frag"

void main() {
    if (params.SMAA_EDT == 0.0) {
        FragColor = vec4(SMAALumaEdgeDetectionPS(vTexCoord, offset, Source), 0.0, 0.0);
    } else if (params.SMAA_EDT <= 1.0) {
        FragColor = vec4(SMAAColorEdgeDetectionPS(vTexCoord, offset, Source), 0.0, 0.0);
    // Unavailable as we don't have access to a depth buffer (yet?)
    // } else if (params.SMAA_EDT <= 2.0) {
    //     FragColor = vec4(SMAADepthEdgeDetectionPS(vTexCoord, offset, depthTex), 0.0, 0.0);
    }
}
