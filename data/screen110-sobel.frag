#version 110

varying vec2 out_tex_coord;

uniform sampler2D tex;
uniform ivec2 tex_size;

const float bw = 1. / 3000.;
const float bh = 1. / 2000.;

void main(void) {
    vec4 s1 = texture2D(tex, out_tex_coord - bw - bh);
    vec4 s2 = texture2D(tex, out_tex_coord + bw - bh);
    vec4 s3 = texture2D(tex, out_tex_coord - bw + bh);
    vec4 s4 = texture2D(tex, out_tex_coord + bw + bh);

    vec4 sx = 4. * ((s4 + s3) - (s2 + s1));
    vec4 sy = 4. * ((s2 + s4) - (s1 + s3));
    gl_FragColor = sqrt(sx * sx + sy * sy);
}
