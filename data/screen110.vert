#version 110

attribute vec2 coord;
attribute vec2 tex_coord;

varying vec2 out_tex_coord;

void main(void) {
    out_tex_coord = tex_coord;
    gl_Position = vec4(coord, 0., 1.);
}
