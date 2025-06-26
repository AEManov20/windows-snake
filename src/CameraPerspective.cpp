#include "CameraPerspective.h"
#include <cmath>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext/matrix_transform.hpp>

glm::vec3 CameraPerspective::GetFrontVector() const
{
    return glm::normalize(glm::vec3 {
        std::cos(glm::radians(m_ViewRotation.x)) * std::cos(glm::radians(m_ViewRotation.y)),
        std::sin(glm::radians(m_ViewRotation.y)),
        std::sin(glm::radians(m_ViewRotation.x)) * std::cos(glm::radians(m_ViewRotation.y)),
    });
}

glm::mat4 CameraPerspective::GetMVPMatrix(glm::mat4 modelMatrix) const
{
    const auto& cameraPos = m_Translation;
    const auto cameraFront = GetFrontVector();
    const auto cameraRight = glm::normalize(glm::cross(cameraFront, c_UpVector));
    const auto cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

    return glm::perspective(m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane) *
        (glm::lookAt(m_Translation, m_Translation + cameraFront, cameraUp)) *
        modelMatrix;
}
