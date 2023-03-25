#version 330 core

in vec2 coord;
in vec2 tex_coord;

out vec2 out_tex_coord;

void main(void) {
    out_tex_coord = tex_coord;
    gl_Position = vec4(coord, 0., 1.);
}
