#version 330 core

in vec2 out_tex_coord;

out vec4 frag_color;

uniform sampler2D tex;

void main(void) {
    vec4 c = texture(tex, out_tex_coord);
    float k = (c.r + c.g + c.b) / 3.;
    frag_color = vec4(k, k, k, 1.);
}
