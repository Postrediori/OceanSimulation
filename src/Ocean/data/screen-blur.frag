#version 330 core

in vec2 out_tex_coord;

out vec4 frag_color;

uniform sampler2D tex;

const float blur_width = 1. / 750.;
const float blur_height = 1. / 750.;

void main(void) {
    vec4 sum = vec4(0.0);
    for (int x=-4; x<=4; x++) {
        for (int y=-4; y<=4; y++) {
            vec2 crd = vec2(out_tex_coord.x+float(x)*blur_width,
                            out_tex_coord.y+float(y)*blur_height);
            sum += texture(tex, crd) / 81.;
        }
    }
    frag_color = sum;
}
