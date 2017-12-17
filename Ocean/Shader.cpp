// Shader.cpp
#include "stdafx.h"
#include "Shader.h"

namespace Shader {

int loadFile(const char *filename, std::string& data) {
    std::ifstream in(filename, std::ios::in);
    if (!in) return 0;

    std::string line;
    std::ostringstream out;
    while (std::getline(in, line))
        out<<line<<std::endl;

    data = out.str();
    if (!data.length()) return 0;

    return 1;
}

void showShaderError(const GLuint shader) {
    GLint   blen = 0;
    GLsizei slen = 0;

    // std::cerr<<"Shader error : ";
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen);
    if (blen>1) {
        GLchar * compiler_log = new GLchar[blen];
        glGetInfoLogARB(shader, blen, &slen, compiler_log);
        std::cerr<<compiler_log;
        delete [] compiler_log;
    } else {
        std::cerr<<"Unknown error";
    }
    std::cerr<<std::endl;
}

void showShaderInfo(const GLuint glProgram, const GLuint glShaderV, const GLuint glShaderF) {
    int vlength, flength, plength;
    glGetObjectParameterivARB(glShaderV, GL_OBJECT_INFO_LOG_LENGTH_ARB, &vlength);
    glGetObjectParameterivARB(glShaderF, GL_OBJECT_INFO_LOG_LENGTH_ARB, &flength);
    glGetObjectParameterivARB(glProgram, GL_OBJECT_INFO_LOG_LENGTH_ARB, &plength);

    if (vlength>1) {
        GLchar * vlog = new GLchar[vlength+1];
        glGetShaderInfoLog(glShaderV, vlength, NULL, vlog);
        std::cout<<"Vertex Shader    : "<<vlog;
        if (vlog[vlength-1]!='\n') std::cout<<std::endl;
        delete [] vlog;
    }

    if (flength>1) {
        GLchar * flog = new GLchar[flength+1];
        glGetShaderInfoLog(glShaderF, flength, NULL, flog);
        std::cout<<"Fragment Shader  : "<<flog;
        if (flog[flength-1]!='\n') std::cout<<std::endl;
        delete [] flog;
    }

    if (plength>1) {
        GLchar * plog = new GLchar[plength+1];
        glGetProgramInfoLog(glProgram, plength, NULL, plog);
        std::cout<<"Shader Program   : "<<plog;
        if (plog[plength-1]!='\n') std::cout<<std::endl;
        delete [] plog;
    }
}

void createProgram(GLuint& glProgram, GLuint& glShaderV, GLuint& glShaderF,
                   const char * vertex_shader, const char * fragment_shader) {
    std::cout<<std::endl<<"Shader Files: "<<vertex_shader<<" "<<fragment_shader<<std::endl;

    std::string strVert, strFrag;
    if (!loadFile(vertex_shader,   strVert)) {
        std::cerr<<"Vertex Shader Error : Unable to load file"<<std::endl;
        return;
    }

    if (!loadFile(fragment_shader, strFrag)) {
        std::cerr<<"Fragment Shader Error : Unable to load file"<<std::endl;
        return;
    }

    const GLchar * sourceVertex   = strVert.c_str();
    const GLchar * sourceFragment = strFrag.c_str();

    createProgramSource(glProgram, glShaderV, glShaderF, sourceVertex, sourceFragment);
}


void createProgramSource(GLuint& glProgram, GLuint& glShaderV, GLuint& glShaderF,
                         const char * vertex_shader, const char * fragment_shader) {
    GLuint program;
    GLuint vShader, fShader;

    glProgram = 0;
    glShaderV = 0;
    glShaderF = 0;

    std::cout<<std::endl;

    vShader = glCreateShader(GL_VERTEX_SHADER);
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vShader, 1, &vertex_shader, NULL);
    glShaderSource(fShader, 1, &fragment_shader, NULL);

    GLint result;

    glCompileShader(vShader);
    glGetObjectParameterivARB(vShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        std::cerr<<"Vertex Shader Error : ";
        showShaderError(vShader);
        glDeleteShader(vShader);
        return;
    }

    glCompileShader(fShader);
    glGetObjectParameterivARB(fShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        std::cerr<<"Fragment Shader Error : ";
        showShaderError(fShader);
        glDeleteShader(vShader);
        glDeleteShader(fShader);
        return;
    }

    program = glCreateProgram();

    glAttachShader(program, vShader);
    glAttachShader(program, fShader);

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (!result) {
        std::cerr<<"Linking Shader Error : ";
        showShaderError(program);
        showShaderInfo(program, vShader, fShader);
        glDetachShader(program, vShader);
        glDetachShader(program, fShader);
        glDeleteShader(vShader);
        glDeleteShader(fShader);
        glDeleteProgram(program);
        return;
    }

    // show shader info
    showShaderInfo(program, vShader, fShader);

    glProgram = program;
    glShaderV = vShader;
    glShaderF = fShader;
}

void releaseProgram(GLuint& glProgram, GLuint& glShaderV, GLuint& glShaderF) {
    glDetachShader(glProgram, glShaderF);
    glDetachShader(glProgram, glShaderV);
    glDeleteShader(glShaderF);
    glDeleteShader(glShaderV);
    glDeleteProgram(glProgram);

    glProgram = 0;
    glShaderV = 0;
    glShaderF = 0;
}

} // namespace
