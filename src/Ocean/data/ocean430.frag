#version 430

in vec3 normal_vector;
in vec3 light_vector;
in vec3 halfway_vector;
// in vec2 tex_coord;
in float fog_factor;

out vec4 frag_col;

// uniform sampler2D water_tex;

// vec4 fog_color      = vec4(0.55, 0.55, 0.55, 1.0);
const vec4 fog_color      = vec4(0.25, 0.75, 0.65, 1.0);
const vec4 emissive_color = vec4(1.0, 1.0, 1.0,  1.0);
const vec4 ambient_color  = vec4(0.0, 0.65, 0.75, 1.0);
const vec4 diffuse_color  = vec4(0.5, 0.65, 0.75, 1.0);
const vec4 specular_color = vec4(1.0, 0.25, 0.0,  1.0);

const float emissive_contribution = 0.00;
const float ambient_contribution  = 0.30;
const float diffuse_contribution  = 0.30;
const float specular_contribution = 1.80;

void main(void) {
    vec3 normal1         = normalize(normal_vector);
    vec3 light_vector1   = normalize(light_vector);
    vec3 halfway_vector1 = normalize(halfway_vector);

    // vec4 c = texture2D(water_tex, tex_coord);
    vec4 c = vec4(1., 1., 1., 1.);

    float d = dot(normal1, light_vector1);
    vec4 emissive_light = emissive_color * emissive_contribution;
    vec4 ambient_light = ambient_color  * ambient_contribution * c;
    vec4 diff_light = diffuse_color  * diffuse_contribution  * c *
        max(d, 0.);

    vec4 specular_light;
    if (d > 0.) {
        specular_light = specular_color * specular_contribution * c *
            max(pow(dot(normal1, halfway_vector1), 120.), 0.);
    }
    else {
        specular_light = vec4(0.);
    }

    frag_col = emissive_light + ambient_light +
        diff_light + specular_light;
    frag_col = mix(frag_col, fog_color, fog_factor);
    frag_col.a = 1.;
}
