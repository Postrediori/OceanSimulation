#version 110

varying vec2 out_tex_coord;

uniform sampler2D tex;
uniform ivec2 tex_size;

void main(void) {
    vec4 c = texture2D(tex, out_tex_coord);
    float k = .2146 * c.r + .7125 * c.g + .0722 * c.b;
    gl_FragColor = vec4(k, k, k, 1.);
}
