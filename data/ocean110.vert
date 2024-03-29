#version 110

attribute vec3 vertex;
attribute vec3 normal;

varying vec3 light_vector;
varying vec3 normal_vector;
varying vec3 halfway_vector;
varying float fog_factor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 mv_transp_inv;
uniform vec3 light_pos;

const float fog_distance = 1000.;

void main(void) {
    gl_Position = view * model * vec4(vertex, 1.0);
    fog_factor = min(-gl_Position.z/fog_distance, 1.0);
    gl_Position = projection * gl_Position;

    vec4 v = view * model * vec4(vertex, 1.0);
    vec3 normal1 = normalize(normal);

    light_vector = normalize((view * vec4(light_pos, 1.)).xyz - v.xyz);
    normal_vector = (mv_transp_inv * vec4(normal1, 0.)).xyz;
    halfway_vector = light_vector + normalize(-v.xyz);
}
