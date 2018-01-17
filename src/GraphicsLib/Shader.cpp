// Shader.cpp
#include "stdafx.h"
#include "Shader.h"

namespace Shader {

std::string loadFile(const std::string& filename) {
    std::ifstream in(filename, std::ios::in);
    if (!in) {
        return "";
    }

    std::string line;
    std::stringstream str;
    while (std::getline(in, line)) {
        str << line << std::endl;
    }

    return str.str();
}

std::string showShaderError(GLuint shader) {
    GLint blen = 0;
    GLsizei slen = 0;
    std::stringstream str;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen);
    if (blen>1) {
        std::vector<char> compiler_log(blen + 1);

        glGetInfoLogARB(shader, blen, &slen, compiler_log.data());

        std::string s(compiler_log.begin(), compiler_log.end());
        str << s;

    } else {
        str << "Unknown error";
    }

    return str.str();
}

std::string showShaderInfo(GLuint shader) {
    int length;
    std::stringstream str;

    glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
    if (length > 1) {
        std::vector<char> log(length + 1);
        glGetShaderInfoLog(shader, length, NULL, log.data());

        std::string s(log.begin(), log.end());
        str << s;
    } else {
        str << "No Shader Info";
    }

    return str.str();
}

std::string showProgramInfo(GLuint program) {
    int length;
    std::stringstream str;

    glGetObjectParameterivARB(program, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
    if (length > 1) {
        std::vector<char> log(length + 1);
        glGetProgramInfoLog(program, length, NULL, log.data());

        std::string s(log.begin(), log.end());
        str << s;
    } else {
        str << "No Program Info";
    }

    return str.str();
}

bool createProgram(GLuint& program, GLuint& vertex, GLuint& fragment,
                   const std::string& vertex_shader, const std::string& fragment_shader) {
    LOGI << "Shader Files: " << vertex_shader << " " << fragment_shader;

    std::string strVert = loadFile(vertex_shader);
    if (strVert.empty()) {
        LOGE << "Vertex Shader Error : Unable to load file";
        return false;
    }

    std::string strFrag = loadFile(fragment_shader);
    if (strFrag.empty()) {
        LOGE << "Fragment Shader Error : Unable to load file";
        return false;
    }

    return createProgramSource(program, vertex, fragment, strVert, strFrag);
}

bool createProgramSource(GLuint& program, GLuint& vertex, GLuint& fragment,
                             const std::string& vertex_shader, const std::string& fragment_shader) {
    LOGD << "Vertex Shader    : " << vertex_shader.length() << " symbols";
    LOGD << "Fragment Shader  : " << fragment_shader.length() << " symbols";

    GLuint sProgram = 0;
    GLuint vShader = 0;
    GLuint fShader = 0;
    GLint result;

    const GLchar* vertexSource = vertex_shader.c_str();
    const GLchar* fragmentSource = fragment_shader.c_str();

    vShader = glCreateShader(GL_VERTEX_SHADER);
    if (!vShader) {
        LOGE << "Unable to Create Vertex Shader";
        goto error;
    }

    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!fShader) {
        LOGE << "Unable to Create Fragment Shader";
        goto error;
    }

    glShaderSource(vShader, 1, &vertexSource, NULL);
    glCompileShader(vShader);
    glGetObjectParameterivARB(vShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        LOGE << "Vertex Shader Error : " << showShaderError(vShader);
        goto error;
    }

    glShaderSource(fShader, 1, &fragmentSource, NULL);
    glCompileShader(fShader);
    glGetObjectParameterivARB(fShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        LOGE << "Fragment Shader Error : " << showShaderError(fShader);
        goto error;
    }

    sProgram = glCreateProgram();
    if (!sProgram) {
        LOGE << "Unable to Create Program";
        goto error;
    }

    glAttachShader(sProgram, vShader);
    glAttachShader(sProgram, fShader);

    glLinkProgram(sProgram);

    glGetProgramiv(sProgram, GL_LINK_STATUS, &result);
    if (!result) {
        LOGE << "Linking Shader Error : " << showShaderError(sProgram);
        goto error;
    }

    // show shader info
    LOGD << "Vertex Shader    : " << showShaderInfo(vShader);
    LOGD << "Fragment Shader  : " << showShaderInfo(fShader);
    LOGD << "Shader Program   : " << showProgramInfo(sProgram);

    program = sProgram;
    vertex = vShader;
    fragment = fShader;

    return true;

error:
    releaseProgram(sProgram, vShader, fShader);

    return false;
}

void releaseProgram(GLuint program, GLuint vertex, GLuint fragment) {
    if (program) {
        if (vertex) {
            glDetachShader(program, vertex);
        }
        if (fragment) {
            glDetachShader(program, fragment);
        }

        glDeleteProgram(program);
    }

    if (vertex) {
        glDeleteShader(vertex);
    }

    if (fragment) {
        glDeleteShader(fragment);
    }
}

} // namespace
