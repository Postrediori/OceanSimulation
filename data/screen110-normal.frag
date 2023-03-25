#version 110

varying vec2 out_tex_coord;

uniform sampler2D tex;
uniform ivec2 tex_size;

void main(void) {
    gl_FragColor = texture2D(tex, out_tex_coord);
}
