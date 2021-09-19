#version 450
#pragma name FSR_RCAS

// FSR - [RCAS] ROBUST CONTRAST ADAPTIVE SHARPENING

// filter_linear1 = true
// scale_type1 = source
// scale1 = 1.0

#pragma parameter FSR_SHARPENING "FSR RCAS Sharpening Amount (Lower = Sharper)" 0.2 0.0 2.0 0.1
#pragma parameter FSR_FILMGRAIN "FSR LFGA Film Grain Intensity" 0.10 0.0 1.0 0.02

layout(push_constant) uniform Push {
    vec4 SourceSize;
    vec4 OriginalSize;
    vec4 OutputSize;
    uint FrameCount;
    float FSR_SHARPENING;
    float FSR_FILMGRAIN;
} params;

layout(std140, set = 0, binding = 0) uniform UBO {
    mat4 MVP;
} global;

#pragma stage vertex
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 0) out vec2 vTexCoord;

void main() {
    gl_Position = global.MVP * Position;
    vTexCoord = TexCoord;
}

#pragma stage fragment
layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 2) uniform sampler2D Source;

#define A_GPU 1
#define A_GLSL 1
#include "ffx_a.h"

#define FSR_RCAS_F 1
AU4 con0;

AF4 FsrRcasLoadF(ASU2 p) {
    return AF4(texelFetch(Source, ASU2(p), 0));
}

void FsrRcasInputF(inout AF1 r, inout AF1 g, inout AF1 b) {}

#include "ffx_fsr1.h"

void main() {
    FsrRcasCon(con0, params.FSR_SHARPENING);

    AU2 gxy = AU2(vTexCoord.xy * params.OutputSize.xy); // Integer pixel position in output.
    AF3 Gamma2Color = AF3(0, 0, 0);
    FsrRcasF(Gamma2Color.r, Gamma2Color.g, Gamma2Color.b, gxy, con0);

    // FSR - [LFGA] LINEAR FILM GRAIN APPLICATOR
    if (params.FSR_FILMGRAIN > 0.0) {
        AF1 noise = fract(10000 * sin(((vTexCoord.x + vTexCoord.y * A_2PI) * params.FrameCount)));
        FsrLfgaF(Gamma2Color, AF3_(noise - 0.5), params.FSR_FILMGRAIN);
    }

    FragColor = vec4(Gamma2Color, 1.0);
}
