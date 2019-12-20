// Shader.h
#ifndef SHADER_H
#define SHADER_H

namespace Shader {
    std::string loadShader(const std::string& filename);

    std::string showShaderInfo(GLuint shader);
    std::string showProgramInfo(GLuint program);

    bool createProgram(GLuint& program, GLuint& vertex, GLuint& fragment,
            const std::string& vertex_shader, const std::string& fragment_shader);
    bool createProgramSource(GLuint& program, GLuint& vertex, GLuint& fragment,
            const std::string& vertex_shader, const std::string& fragment_shader);

    void releaseProgram(GLuint program, GLuint vertex, GLuint fragment);
}

#endif
