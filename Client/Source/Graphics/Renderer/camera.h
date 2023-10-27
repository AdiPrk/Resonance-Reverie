#pragma once

#include <PCH/pch.h>
#include <Source/Math/rect.h>
#include <Source/Graphics/Window/window.h>
#include <Source/Graphics/Particles/particleEmitter.h>

class Camera {
public:

    Camera() 
        : m_CameraSize(SCREEN_WIDTH, SCREEN_HEIGHT)
        , m_BoundingRect()
    {
    }

    glm::mat4 GetViewMatrix() {
        glm::mat4 view = glm::mat4(1.0f);

        view = glm::translate(view, { m_CameraSize.x / 2.f, m_CameraSize.y / 2.f, 1.f });
        view = glm::scale(view, glm::vec3(m_Zoom, m_Zoom, 1.0f));
        view = glm::translate(view, glm::vec3(-m_Position, 0.0f));

        return view;
    }

    void UpdateUniforms() {
        glm::mat4 view = GetViewMatrix();
        ResourceManager::GetShader("sprite").Use().SetMatrix4("view", view);
        ResourceManager::GetShader("spritetiles").Use().SetMatrix4("view", view);
        ResourceManager::GetShader("spriteflagtiles").Use().SetMatrix4("view", view);
        ResourceManager::GetShader("spriteillbeback").Use().SetMatrix4("view", view);
        ResourceManager::GetShader("spritelava").Use().SetMatrix4("view", view);
        ResourceManager::GetShader("particle").Use().SetMatrix4("view", view);
        ResourceManager::GetShader("text").Use().SetMatrix4("view", view);
    }

    void CalculateBoundingRect() {
        m_BoundingRect.width = m_CameraSize.x / m_Zoom;
        m_BoundingRect.height = m_CameraSize.y / m_Zoom;

        m_BoundingRect.left = m_Position.x - m_BoundingRect.width / 2;
        m_BoundingRect.top = m_Position.y - m_BoundingRect.height / 2;

        m_BoundingRect.right = m_BoundingRect.left + m_BoundingRect.width;
        m_BoundingRect.bottom = m_BoundingRect.top + m_BoundingRect.height;
    }

    void MoveTo(glm::vec2 moveTo, Rect bounds) {
        if (bounds.width >= m_BoundingRect.width) // to avoid crashing with clamp
        {
            moveTo.x = std::clamp(moveTo.x, bounds.left + m_BoundingRect.width / 2.f, bounds.right - m_BoundingRect.width / 2.f);
            moveTo.y = std::clamp(moveTo.y, bounds.top + m_BoundingRect.height / 2.f, bounds.bottom - m_BoundingRect.height / 2.f);
        }

        m_Position = moveTo;
    }

    void GlideTo(glm::vec2 glideTo, Rect bounds, float dt) {
        float targetDiff = fabsf(m_TargetZoom - m_Zoom);
        m_Zoom = lerp(m_Zoom, m_TargetZoom, (2.5f + targetDiff) * dt);
        if (fabsf(m_Zoom - m_TargetZoom) < 0.0005f) m_Zoom = m_TargetZoom;

        CalculateBoundingRect();

        if (bounds.width >= m_BoundingRect.width) // to avoid crashing with clamp
        {
            glideTo.x = std::clamp(glideTo.x, bounds.left + m_BoundingRect.width / 2.f, bounds.right - m_BoundingRect.width / 2.f);
            glideTo.y = std::clamp(glideTo.y, bounds.top + m_BoundingRect.height / 2.f, bounds.bottom - m_BoundingRect.height / 2.f);
        }

        if (glm::distance(glideTo, m_Position) > 0.1f) {
            float speed = 5.0f * dt;
            m_Position += (glideTo - m_Position) * speed;
        }
        else {
            m_Position = glideTo;
        }

        CalculateBoundingRect();
    }

    const Rect Bounds() const { return m_BoundingRect; }
    void SetScale(float scale) { m_TargetZoom = scale; }

    const bool IsZoomEqualTarget() const { return m_Zoom == m_TargetZoom; }

private:
    Rect m_BoundingRect;
    glm::vec2 m_Position = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_CameraSize;
    float m_Zoom = 1.0f;
    float m_TargetZoom = 1.0f;
};