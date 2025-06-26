#include "CameraPerspective.h"
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext/matrix_transform.hpp>

glm::vec3 CameraPerspective::GetFrontVector() const
{
    return glm::normalize(glm::vec3 {
        m_Translation.x + (std::cos(glm::radians(m_ViewRotation.x)) * std::cos(glm::radians(m_ViewRotation.y))),
        m_Translation.y + std::sin(glm::radians(m_ViewRotation.y)),
        m_Translation.z - (std::sin(glm::radians(m_ViewRotation.x)) * std::cos(glm::radians(m_ViewRotation.y))),
    });
}

glm::mat4 CameraPerspective::GetMVPMatrix(glm::mat4 modelMatrix) const
{
    return glm::perspective(m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane) *
        (glm::lookAt(m_Translation, GetFrontVector(), c_UpVector)) *
        modelMatrix;
}
