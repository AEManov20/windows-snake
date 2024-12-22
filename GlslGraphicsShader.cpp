//
// Created by alex on 30.11.24.
//

#include "GlslGraphicsShader.h"

#include <stdexcept>
#include <utility>
#include <vector>
#include <epoxy/gl.h>

GlslGraphicsShader::GlslGraphicsShader(const std::string &vertexSource, const std::string &fragmentSource) {
    const GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    const GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint linkResult;
    GLint infoLogLength;

    glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (linkResult != GL_TRUE) {
        std::vector<GLchar> infoLog(static_cast<size_t>(infoLogLength) + 1);
        glGetProgramInfoLog(program, infoLogLength, nullptr, &infoLog[0]);

        throw std::runtime_error("Program linking failed: " + std::string(infoLog.begin(), infoLog.end()));
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    m_Program = program;
}

GlslGraphicsShader::GlslGraphicsShader(GlslGraphicsShader &&other) noexcept :
    m_Program(std::exchange(other.m_Program, 0)) {}

GlslGraphicsShader & GlslGraphicsShader::operator=(GlslGraphicsShader &&other) noexcept {
    GlslGraphicsShader temp(std::move(other));
    std::swap(m_Program, temp.m_Program);

    return *this;
}

void GlslGraphicsShader::Bind() {
    glUseProgram(m_Program);
}

void GlslGraphicsShader::Unbind() {
    glUseProgram(0);
}

#define IMPL_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, GL_TYPE, SIZE, ...) \
    void GlslGraphicsShader::SetUniform##CAPITALIZED_TYPE##SIZE(const std::string &name, const glm::vec<SIZE, TYPE> v) { \
        const GLint location = glGetUniformLocation(m_Program, name.c_str());                                            \
        if (location == -1) {                                                                                            \
            throw std::runtime_error("Uniform '" + name + "' not found");                                                \
        }                                                                                                                \
        glProgramUniform##GL_TYPE(m_Program, location, __VA_ARGS__);                                                     \
    }

#define IMPL_UNIFORM_VEC_ALL(CAPITALIZED_TYPE, TYPE, GL_TYPE) \
    IMPL_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 1##GL_TYPE, 1, v.x)                 \
    IMPL_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 2##GL_TYPE, 2, v.x, v.y)            \
    IMPL_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 3##GL_TYPE, 3, v.x, v.y, v.z)       \
    IMPL_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 4##GL_TYPE, 4, v.x, v.y, v.z, v.w)  \

IMPL_UNIFORM_VEC_ALL(Float, float, f)
IMPL_UNIFORM_VEC_ALL(Int, int, i)
IMPL_UNIFORM_VEC_ALL(UInt, uint, ui)

void GlslGraphicsShader::SetUniformMatrix4x4(const std::string &name, const glm::mat4 &matrix) {
    const GLint location = glGetUniformLocation(m_Program, name.c_str());
    if (location == -1) {
        throw std::runtime_error("Uniform '" + name + "' not found");
    }
    glProgramUniformMatrix4fv(m_Program, location, 1, false, &matrix[0][0]);
}


GlslGraphicsShader::~GlslGraphicsShader() {
    if (m_Program != 0) {
        glDeleteProgram(m_Program);
    }
}

GLuint GlslGraphicsShader::CompileShader(const GLenum shaderType, const std::string &source) {
    const GLuint shader = glCreateShader(shaderType);

    GLint compilationStatus = GL_FALSE;
    GLint infoLogLength = 0;

    char const *sourcePtr = source.c_str();
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationStatus);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (compilationStatus == GL_FALSE) {
        std::vector<GLchar> infoLog(static_cast<size_t>(infoLogLength) + 1);
        glGetShaderInfoLog(shader, infoLogLength, nullptr, &infoLog[0]);

        throw std::runtime_error("Shader compilation failed: " + std::string(infoLog.begin(), infoLog.end()));
    }

    return shader;
}
