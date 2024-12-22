//
// Created by alex on 30.11.24.
//

#ifndef GLGRAPHICSSHADER_H
#define GLGRAPHICSSHADER_H
#include <string>
#include <epoxy/gl.h>

#include "GraphicsShader.h"

class GlGraphicsShader final : public GraphicsShader
{
public:
    GlGraphicsShader(const std::string &vertexSource, const std::string &fragmentSource);

    GlGraphicsShader(GlGraphicsShader &&other) noexcept;

    GlGraphicsShader &operator=(GlGraphicsShader &&other) noexcept;

    GlGraphicsShader(const GlGraphicsShader &other) = delete;

    GlGraphicsShader &operator=(const GlGraphicsShader &) = delete;

    void Bind() override;

    void Unbind() override;

    DECLARE_OVERRIDE_UNIFORM_VEC_ALL(Float, float)

    DECLARE_OVERRIDE_UNIFORM_VEC_ALL(Int, int)

    DECLARE_OVERRIDE_UNIFORM_VEC_ALL(UInt, uint)

    void SetUniformMatrix4x4(const std::string &name, const glm::mat4 &matrix) override;

    [[nodiscard]] GLuint GetProgramID() const { return m_Program; }

    ~GlGraphicsShader() override;

private:
    static GLuint CompileShader(GLenum shaderType, const std::string &source);

private:
    GLuint m_Program = 0;
};


#endif // GLGRAPHICSSHADER_H
