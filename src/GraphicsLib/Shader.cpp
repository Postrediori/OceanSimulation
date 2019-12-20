// Shader.cpp
#include "stdafx.h"
#include "Shader.h"

std::string Shader::loadShader(const std::string& filename) {
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

std::string Shader::showShaderInfo(GLuint shader) {
    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length <= 1) {
        return "No Info";
    }

    std::vector<char> log(length + 1);
    glGetShaderInfoLog(shader, length, NULL, log.data());

    std::string str(log.begin(), log.end());
    return str;
}

std::string Shader::showProgramInfo(GLuint program) {
    int length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    if (length <= 1) {
        return "No Info";
    }

    std::vector<char> log(length + 1);
    glGetProgramInfoLog(program, length, NULL, log.data());

    std::string str(log.begin(), log.end());
    return str;
}

bool Shader::createProgram(GLuint& program, GLuint& vertex, GLuint& fragment,
                   const std::string& vertex_shader, const std::string& fragment_shader) {
    LOGI << "Shader Files: " << vertex_shader << " " << fragment_shader;

    std::string strVert = Shader::loadShader(vertex_shader);
    if (strVert.empty()) {
        LOGE << "Vertex Shader Error : Unable to load file";
        return false;
    }

    std::string strFrag = Shader::loadShader(fragment_shader);
    if (strFrag.empty()) {
        LOGE << "Fragment Shader Error : Unable to load file";
        return false;
    }

    return createProgramSource(program, vertex, fragment, strVert, strFrag);
}

bool Shader::createProgramSource(GLuint& program, GLuint& vertex, GLuint& fragment,
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
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        LOGE << "Vertex Shader Error : " << Shader::showShaderInfo(vShader);
        goto error;
    }

    glShaderSource(fShader, 1, &fragmentSource, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        LOGE << "Fragment Shader Error : " << Shader::showShaderInfo(fShader);
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
        LOGE << "Linking Shader Program Error : " << Shader::showProgramInfo(sProgram);
        goto error;
    }

    // show shader info
    LOGD << "Vertex Shader    : " << Shader::showShaderInfo(vShader);
    LOGD << "Fragment Shader  : " << Shader::showShaderInfo(fShader);
    LOGD << "Shader Program   : " << Shader::showProgramInfo(sProgram);

    program = sProgram;
    vertex = vShader;
    fragment = fShader;

    return true;

error:
    releaseProgram(sProgram, vShader, fShader);

    return false;
}

void Shader::releaseProgram(GLuint program, GLuint vertex, GLuint fragment) {
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
