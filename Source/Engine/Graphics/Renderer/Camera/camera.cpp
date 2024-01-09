#include <Engine/PCH/pch.h>
#include "camera.h"

#include <Engine/Graphics/Shader/shader.h>

namespace Dog {

    Camera::Camera()
        : m_CameraSize(Window::GetWidth(), Window::GetHeight())
        , m_BoundingRect() {}

    Camera::Camera(const glm::vec2& pos)
        : m_CameraSize(Window::GetWidth(), Window::GetHeight())
        , m_BoundingRect()
        , m_Position(pos) {}

    glm::mat4 Camera::GetViewMatrix() {
        glm::mat4 view = glm::mat4(1.0f);

        view = glm::translate(view, { m_CameraSize * 0.5f, 0.f });
        view = glm::scale(view, glm::vec3(m_Zoom, m_Zoom, 1.0f));
        view = glm::translate(view, glm::vec3(-m_Position, 0.0f));

        return view;
    }

    void Camera::UpdateUniforms() {
        glm::mat4 view = GetViewMatrix();
        glm::mat4 proj = glm::ortho(0.f, float(Window::GetWidth()), float(Window::GetHeight()), 0.f, -1.0f, 1.0f);
        glm::mat4 projView = proj * view;

        Shader::SetViewAndProjectionView(view, projView);
    }

    void Camera::CalculateBoundingRect() {
        m_BoundingRect.width = m_CameraSize.x / m_Zoom;
        m_BoundingRect.height = m_CameraSize.y / m_Zoom;

        m_BoundingRect.left = m_Position.x - m_BoundingRect.width / 2;
        m_BoundingRect.top = m_Position.y - m_BoundingRect.height / 2;

        m_BoundingRect.right = m_BoundingRect.left + m_BoundingRect.width;
        m_BoundingRect.bottom = m_BoundingRect.top + m_BoundingRect.height;
    }

    void Camera::MoveTo(glm::vec2 moveTo) {
        m_Position = moveTo;
    }

    void Camera::MoveTo(glm::vec2 moveTo, Rect bounds) {
        CalculateBoundingRect();

        if (bounds.width >= m_BoundingRect.width) // to avoid crashing with clamp
        {
            moveTo.x = std::clamp(moveTo.x, bounds.left + m_BoundingRect.width / 2.f, bounds.right - m_BoundingRect.width / 2.f);
            moveTo.y = std::clamp(moveTo.y, bounds.top + m_BoundingRect.height / 2.f, bounds.bottom - m_BoundingRect.height / 2.f);
        }

        m_Position = moveTo;
    }

    void Camera::UpdateZoom(float dt) {
        m_Zoom = Lerp(m_Zoom, m_TargetZoom, 6.0f * dt);
        if (fabsf(m_Zoom - m_TargetZoom) < 0.0005f) m_Zoom = m_TargetZoom;
        CalculateBoundingRect();
    }

    void Camera::GlideTo(glm::vec2 glideTo, Rect bounds, float dt) {
        if (bounds.width >= m_BoundingRect.width) // to avoid crashing with clamp
        {
            glideTo.x = std::clamp(glideTo.x, bounds.left + m_BoundingRect.width / 2.f, bounds.right - m_BoundingRect.width / 2.f);
            glideTo.y = std::clamp(glideTo.y, bounds.top + m_BoundingRect.height / 2.f, bounds.bottom - m_BoundingRect.height / 2.f);
        }
        else {
            glideTo.x = std::clamp(glideTo.x, bounds.right - m_BoundingRect.width / 2.f, bounds.left + m_BoundingRect.width / 2.f);
            glideTo.y = std::clamp(glideTo.y, bounds.bottom - m_BoundingRect.height / 2.f, bounds.top + m_BoundingRect.height / 2.f);
        }

        if (glm::distance(glideTo, m_Position) > 0.1f) {
            float distance = glm::distance(glideTo, m_Position);
            float baseSpeed = 50.0f; // Base speed
            float minSpeed = 10.0f; // Minimum speed to maintain
            float speedFactor = 0.2f; // Adjust this factor to control deceleration

            // Calculate speed based on distance
            float speed = baseSpeed * (1 - powf(distance / speedFactor, 2));
            speed = glm::max(speed, minSpeed); // Ensure speed doesn't fall below minSpeed

            // Update position
            m_Position += (glideTo - m_Position) * speed * dt;
        }
        else {
            m_Position = glideTo;
        }

        CalculateBoundingRect();
    }

}