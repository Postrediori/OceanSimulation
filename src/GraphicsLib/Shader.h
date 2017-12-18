// Shader.h
#ifndef SHADER_H
#define SHADER_H

namespace Shader {
int loadShader(const char *filename, std::string& data);
void showShaderError(const GLuint shader);
void showShaderInfo(const GLuint glProgram, const GLuint glShaderV, const GLuint glShaderF);
void createProgram(GLuint& glProgram, GLuint& glShaderV, GLuint& glShaderF,
                   const char * vertex_shader, const char * fragment_shader);
void createProgramSource(GLuint& glProgram, GLuint& glShaderV, GLuint& glShaderF,
                         const char * vertex_shader, const char * fragment_shader);
void releaseProgram(GLuint& glProgram, GLuint& glShaderV, GLuint& glShaderF);
}

#endif
