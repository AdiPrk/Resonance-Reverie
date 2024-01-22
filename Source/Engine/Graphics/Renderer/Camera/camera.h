#pragma once

#include <Engine/PCH/pch.h>
#include <Engine/Graphics/Window/window.h>

namespace Dog {

    class Camera {
    public:

        Camera();
        Camera(const glm::vec2& pos);

        glm::mat4 GetViewMatrix();

        void UpdateUniforms();

        void CalculateBoundingRect();

        void UpdateZoom(float dt);

        void MoveTo(glm::vec2 moveTo);
        void MoveTo(glm::vec2 moveTo, Rect bounds);

        void GlideTo(glm::vec2 glideTo, float dt);

        const Rect Bounds() const { return m_BoundingRect; }
        void SetScale(float scale) { m_TargetZoom = scale; }
        void SetScaleInstant(float scale) { m_TargetZoom = scale; m_Zoom = scale; }

        const bool IsZoomEqualTarget() const { return m_Zoom == m_TargetZoom; }

        void SetMovementBounds(const Rect& bounds) {
            m_MovementBounds = bounds;
        }

    private:
        Rect m_MovementBounds;
        Rect m_BoundingRect;
        glm::vec2 m_Position = glm::vec2(0.0f, 0.0f);
        glm::vec2 m_CameraSize;
        float m_Zoom = 1.0f;
        float m_TargetZoom = 1.0f;
    };

}