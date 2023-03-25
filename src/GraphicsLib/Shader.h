#pragma once

namespace Shader {
    GLuint CreateProgramFromFiles(
            const std::string& vertex_shader, const std::string& fragment_shader);
    GLuint CreateProgramFromSources(
            const std::string& vertex_shader, const std::string& fragment_shader);
}
