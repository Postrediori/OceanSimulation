#version 110

varying vec2 out_tex_coord;

uniform sampler2D tex;
uniform ivec2 tex_size;

const float blur_width = 1. / 750.;
const float blur_height = 1. / 750.;

void main(void) {
    vec4 sum = vec4(0.0);
    for (int x=-4; x<=4; x++) {
        for (int y=-4; y<=4; y++) {
            vec2 crd = vec2(out_tex_coord.x+float(x)*blur_width,
                            out_tex_coord.y+float(y)*blur_height);
            sum += texture2D(tex, crd) / 81.;
        }
    }
    gl_FragColor = sum;
}
