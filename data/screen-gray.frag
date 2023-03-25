#version 330 core

in vec2 out_tex_coord;

out vec4 frag_color;

uniform sampler2D tex;

void main(void) {
    vec4 c = texture(tex, out_tex_coord);
    float k = .2146 * c.r + .7125 * c.g + .0722 * c.b;
    frag_color = vec4(k, k, k, 1.);
}
