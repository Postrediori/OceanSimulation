#version 330 core

#define PI 3.1415926

in vec2 out_tex_coord;

out vec4 frag_color;

uniform float cnt;
uniform sampler2D tex;

void main(void) {
    vec2 crd = out_tex_coord;
    crd.x += sin(crd.y * 4.*2.*PI + cnt) / 100.;
    frag_color = texture(tex, crd);
}
