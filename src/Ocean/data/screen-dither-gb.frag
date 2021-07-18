#version 330 core

in vec2 out_tex_coord;

out vec4 frag_color;

uniform sampler2D tex;

// Ordered Dithering Shader by mooglemoogle
// https://www.shadertoy.com/view/4lSSRR

const float Threshold = 0.5;
const float Multiplicator = 1.0 / 17.0;
const mat4 DitherMatrix = (mat4(
    1, 13, 4, 16,
    9, 5, 12, 8,
    3, 15, 2, 14,
    11, 7, 10, 6
) - 8.) * Multiplicator;

const mat4 Palette = mat4(
    15, 56, 15, 255,
    48, 98, 48, 255,
    139, 172, 15, 255,
    202, 220, 159, 255
) / 255.;

float GetLuminance(vec4 c) {
    return (0.2126*c.r + 0.7152*c.g + 0.0722*c.b);
}

float AdjustDither( float val, vec2 coord ) {
    vec2 coordMod = mod(coord, 4.0);
    int xMod = int(coordMod.x);
    int yMod = int(coordMod.y);

    vec4 col;
    if (xMod == 0) col = DitherMatrix[0];
    else if (xMod == 1) col = DitherMatrix[1];
    else if (xMod == 2) col = DitherMatrix[2];
    else if (xMod == 3) col = DitherMatrix[3];

    float adjustment = 0.;
    if (yMod == 0) adjustment = col.x;
    else if (yMod == 1) adjustment = col.y;
    else if (yMod == 2) adjustment = col.z;
    else if (yMod == 3) adjustment = col.w;

    return val + (val * adjustment);
}

void main(void) {
    vec4 sourceColor = texture(tex, out_tex_coord);
    float luminance = GetLuminance(sourceColor);

    ivec2 size = textureSize(tex, 0);

    float dither = AdjustDither(luminance, out_tex_coord*vec2(size));
    frag_color = Palette[int(dither * 4.)];
}
