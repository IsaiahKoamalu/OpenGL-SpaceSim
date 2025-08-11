#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"

struct CameraComponent
{
        glm::vec3 pos{0.0f, 1.6f, 3.0f}; // eye height(ish)
        float yaw = -90.0f;
        float pitch = 0.0f;
        float fov = 60.0f;

        glm::vec3 front{0, 0, -1}, up{0, 1, 0}, right{1, 0, 0};
        float speed = 4.0f;
        float sensitivity = 0.1f;

        void updateVectors()
        {
                float cy = cos(glm::radians(yaw));
                float sy = sin(glm::radians(yaw));
                float cp = cos(glm::radians(pitch));
                float sp = sin(glm::radians(pitch));
                front = glm::normalize(glm::vec3(cy*cp, sp, sy*cp));
                right = glm::normalize(glm::cross(front, glm::vec3(0,1,0)));
                up = glm::normalize(glm::cross(right, front));
        }

        glm::mat4 view() const
        {
                return glm::lookAt(pos, pos + front, up);
        }
};

#endif