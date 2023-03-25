#version 330 core

in vec2 out_tex_coord;

out vec4 frag_color;

uniform sampler2D tex;

const float bw = 1. / 3000.;
const float bh = 1. / 2000.;

void main(void) {
    vec4 s1 = texture(tex, out_tex_coord - bw - bh);
    vec4 s2 = texture(tex, out_tex_coord + bw - bh);
    vec4 s3 = texture(tex, out_tex_coord - bw + bh);
    vec4 s4 = texture(tex, out_tex_coord + bw + bh);

    vec4 sx = 4. * ((s4 + s3) - (s2 + s1));
    vec4 sy = 4. * ((s2 + s4) - (s1 + s3));
    frag_color = sqrt(sx * sx + sy * sy);
}
