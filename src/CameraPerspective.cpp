#include "CameraPerspective.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext/matrix_transform.hpp>

glm::mat4 CameraPerspective::GetMVPMatrix(glm::mat4 modelMatrix) const
{
    return glm::perspective(m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane) *
        (glm::lookAt(m_Translation, glm::vec3(0.F, 0.F, 0.F), c_UpVector)) *
        modelMatrix;
}
