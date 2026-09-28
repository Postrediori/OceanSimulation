#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

out vec3 light_vector;
out vec3 normal_vector;
out vec3 halfway_vector;
out float fog_factor;

layout(std140) uniform VertexParams { 
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 light_pos;
};

const float fog_distance = 1000.;

void main(void) {
    gl_Position = view * model * vec4(vertex, 1.0);
    fog_factor = min(-gl_Position.z/fog_distance, 1.0);
    gl_Position = projection * gl_Position;

    vec4 v = view * model * vec4(vertex, 1.0);
    vec3 normal1 = normalize(normal);

    light_vector = normalize((view * light_pos).xyz - v.xyz);
    mat4 mv_transp_inv = inverse(transpose(view * model));
    normal_vector = (mv_transp_inv * vec4(normal1, 0.)).xyz;
    halfway_vector = light_vector + normalize(-v.xyz);
}
