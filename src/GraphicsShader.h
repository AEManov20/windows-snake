//
// Created by alex on 27.11.24.
//

#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <glm/glm.hpp>

#define DECLARE_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, SIZE) \
    virtual void SetUniform##CAPITALIZED_TYPE##SIZE(const std::string &name, const glm::vec<SIZE, TYPE> val) = 0;

#define DECLARE_OVERRIDE_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, SIZE) \
    void SetUniform##CAPITALIZED_TYPE##SIZE(const std::string &name, const glm::vec<SIZE, TYPE> val) override;

#define DECLARE_UNIFORM_VEC_ALL(CAPITALIZED_TYPE, TYPE) \
    DECLARE_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 1) \
    DECLARE_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 2) \
    DECLARE_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 3) \
    DECLARE_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 4)
#define DECLARE_OVERRIDE_UNIFORM_VEC_ALL(CAPITALIZED_TYPE, TYPE) \
    DECLARE_OVERRIDE_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 1) \
    DECLARE_OVERRIDE_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 2) \
    DECLARE_OVERRIDE_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 3) \
    DECLARE_OVERRIDE_UNIFORM_VEC(CAPITALIZED_TYPE, TYPE, 4)

class GraphicsShader
{
public:
  GraphicsShader(const GraphicsShader &) = default;

  GraphicsShader(GraphicsShader &&) = delete;

  GraphicsShader &operator=(const GraphicsShader &) = default;

  GraphicsShader &operator=(GraphicsShader &&) = delete;

  virtual ~GraphicsShader() = default;

  GraphicsShader() = default;
  
  virtual void Bind() = 0;

  virtual void Unbind() = 0;

  DECLARE_UNIFORM_VEC_ALL(Float, float)

  DECLARE_UNIFORM_VEC_ALL(Int, int)

  DECLARE_UNIFORM_VEC_ALL(UInt, uint)

  virtual void SetUniformMatrix4x4(const std::string &name,
                                   const glm::mat4 &matrix) = 0;
};

#endif //SHADER_H
