#version 330 core

in vec2 out_tex_coord;

out vec4 frag_color;

uniform sampler2D tex;

void main(void) {
    frag_color = texture(tex, out_tex_coord);
}
