//
// Created by alex on 30.11.24.
//

#ifndef GLSLGRAPHICSSHADER_H
#define GLSLGRAPHICSSHADER_H
#include <string>
#include <epoxy/gl.h>

#include "GraphicsShader.h"

class GlslGraphicsShader final : GraphicsShader {
public:
    GlslGraphicsShader(const std::string& vertexSource, const std::string& fragmentSource);
    GlslGraphicsShader(GlslGraphicsShader&& other) noexcept;
    GlslGraphicsShader& operator=(GlslGraphicsShader&& other) noexcept;

    GlslGraphicsShader(const GlslGraphicsShader& other) = delete;
    GlslGraphicsShader& operator=(const GlslGraphicsShader&) = delete;

    void Bind() override;
    void Unbind() override;

    DECLARE_OVERRIDE_UNIFORM_VEC_ALL(Float, float)
    DECLARE_OVERRIDE_UNIFORM_VEC_ALL(Int, int)
    DECLARE_OVERRIDE_UNIFORM_VEC_ALL(UInt, uint)

    void SetUniformMatrix4x4(const std::string &name, const glm::mat4 &matrix) override;

    [[nodiscard]] GLuint GetProgramID() const { return m_Program; }

    ~GlslGraphicsShader() override;

private:
    static GLuint CompileShader(GLenum shaderType, const std::string& source);

private:
    GLuint m_Program = 0;
};



#endif // GLSLGRAPHICSSHADER_H
