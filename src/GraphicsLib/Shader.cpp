// Shader.cpp
#include "stdafx.h"
#include "GraphicsLogger.h"
#include "Shader.h"

std::string LoadShaderFile(const std::string& filename) {
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

std::string ShowShaderInfo(GLuint shader) {
    int length{0};

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length); LOGOPENGLERROR();
    if (length == 0) {
        return "";
    }

    std::vector<char> buffer(length);
    glGetShaderInfoLog(shader, length, NULL, buffer.data()); LOGOPENGLERROR();

    std::string str(buffer.begin(), buffer.end());
    return str;
}

std::string ShowProgramInfo(GLuint program) {
    int length{0};

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length); LOGOPENGLERROR();
    if (length == 0) {
        return "";
    }

    std::vector<char> buffer(length);
    glGetProgramInfoLog(program, length, NULL, buffer.data()); LOGOPENGLERROR();

    std::string str(buffer.begin(), buffer.end());
    return str;
}

GLuint Shader::CreateProgramFromFiles(
                   const std::string& vertex_shader, const std::string& fragment_shader) {
    LOGI << "Shader Files: " << vertex_shader << " " << fragment_shader;

    std::string strVert = LoadShaderFile(vertex_shader);
    if (strVert.empty()) {
        LOGE << "Vertex Shader Error : Unable to load file";
        return 0;
    }

    std::string strFrag = LoadShaderFile(fragment_shader);
    if (strFrag.empty()) {
        LOGE << "Fragment Shader Error : Unable to load file";
        return 0;
    }

    return CreateProgramFromSources(strVert, strFrag);
}

GLuint Shader::CreateProgramFromSources(
                             const std::string& vertex_shader, const std::string& fragment_shader) {
    LOGD << "Vertex Shader    : " << vertex_shader.length() << " symbols";
    LOGD << "Fragment Shader  : " << fragment_shader.length() << " symbols";

    GLint result{0};
    GLuint vShader = 0;
    GLuint fShader = 0;
    GLuint sProgram = 0;
    const GLchar* vertexSource = vertex_shader.c_str();
    const GLchar* fragmentSource = fragment_shader.c_str();

    vShader = glCreateShader(GL_VERTEX_SHADER); LOGOPENGLERROR();
    if (!vShader) {
        LOGE << "Unable to Create Vertex Shader";
        goto error;
    }

    fShader = glCreateShader(GL_FRAGMENT_SHADER); LOGOPENGLERROR();
    if (!fShader) {
        LOGE << "Unable to Create Fragment Shader";
        goto error;
    }

    glShaderSource(vShader, 1, &vertexSource, NULL); LOGOPENGLERROR();
    glCompileShader(vShader); LOGOPENGLERROR();
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &result); LOGOPENGLERROR();
    if (!result) {
        LOGE << "Vertex Shader Error : " << ShowShaderInfo(vShader);
        goto error;
    }

    glShaderSource(fShader, 1, &fragmentSource, NULL); LOGOPENGLERROR();
    glCompileShader(fShader); LOGOPENGLERROR();
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &result); LOGOPENGLERROR();
    if (!result) {
        LOGE << "Fragment Shader Error : " << ShowShaderInfo(fShader);
        goto error;
    }

    sProgram = glCreateProgram(); LOGOPENGLERROR();
    if (!sProgram) {
        LOGE << "Unable to Create Program";
        goto error;
    }

    glAttachShader(sProgram, vShader); LOGOPENGLERROR();
    glAttachShader(sProgram, fShader); LOGOPENGLERROR();

    glLinkProgram(sProgram); LOGOPENGLERROR();

    glGetProgramiv(sProgram, GL_LINK_STATUS, &result); LOGOPENGLERROR();
    if (!result) {
        LOGE << "Linking Shader Error : " << ShowProgramInfo(sProgram);
        goto error;
    }

    // Delete shader objects to free memory
    glDeleteShader(vShader); LOGOPENGLERROR();
    glDeleteShader(fShader); LOGOPENGLERROR();

    return sProgram;

error:
    glDeleteShader(vShader); LOGOPENGLERROR();
    glDeleteShader(fShader); LOGOPENGLERROR();
    glDeleteProgram(sProgram); LOGOPENGLERROR();

    return 0;
}
