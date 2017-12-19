// Shader.h
#ifndef SHADER_H
#define SHADER_H

namespace Shader {
    std::string loadShader(const std::string& filename);

    std::string showShaderError(GLuint shader);
    std::string showShaderInfo(GLuint shader);
    std::string showShaderProgram(GLuint program);
    std::string showShaderProgramInfo(GLuint program, GLuint vertex, GLuint fragment);

    bool createProgram(GLuint& program, GLuint& vertex, GLuint& fragment,
            const std::string& vertex_shader, const std::string& fragment_shader);
    bool createProgramSource(GLuint& program, GLuint& vertex, GLuint& fragment,
            const std::string& vertex_shader, const std::string& fragment_shader);

    void releaseProgram(GLuint program, GLuint vertex, GLuint fragment);
}

#endif
