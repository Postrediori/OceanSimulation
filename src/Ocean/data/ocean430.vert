#version 430

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
// in vec3 texture;

out vec3 light_vector;
out vec3 normal_vector;
out vec3 halfway_vector;
// out vec2 tex_coord;
out float fog_factor;

layout (location = 0) uniform mat4 projection;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 model;
//uniform mat4 mv_transp_inv;
layout (location = 3) uniform vec3 light_pos;

const float fog_distance = 1000.;

void main(void) {
    gl_Position = view * model * vec4(vertex, 1.0);
    fog_factor = min(-gl_Position.z/fog_distance, 1.0);
    gl_Position = projection * gl_Position;

    vec4 v = view * model * vec4(vertex, 1.0);
    vec3 normal1 = normalize(normal);

    light_vector = normalize((view * vec4(light_pos, 1.)).xyz - v.xyz);
    mat4 mv_transp_inv = inverse(transpose(view * model));
    normal_vector = (mv_transp_inv * vec4(normal1, 0.)).xyz;
    halfway_vector = light_vector + normalize(-v.xyz);
    // tex_coord = texture.xy;
}
