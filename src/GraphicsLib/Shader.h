#pragma once

namespace Shader {
    std::string loadShaderFile(const std::string& filename);

    std::string showShaderInfo(GLuint shader);
    std::string showProgramInfo(GLuint program);

    bool createProgram(GLuint& program,
            const std::string& vertex_shader, const std::string& fragment_shader);
    bool createProgramSource(GLuint& program,
            const std::string& vertex_shader, const std::string& fragment_shader);

    void releaseProgram(GLuint program, GLuint vertex, GLuint fragment);
}
