#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

const glm::vec3 c_UpVector{ 0.F, 1.F, 0.F };

struct CameraPerspective
{
    // camera parameters
    float m_FieldOfView;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;

    // transform
    glm::vec3 m_Translation;
    glm::vec3 m_ViewRotation;

    // methods
    [[nodiscard]] glm::mat4 GetMVPMatrix(glm::mat4 modelMatrix) const;
    [[nodiscard]] glm::vec3 GetFrontVector() const;
};

#endif // CAMERA_H
