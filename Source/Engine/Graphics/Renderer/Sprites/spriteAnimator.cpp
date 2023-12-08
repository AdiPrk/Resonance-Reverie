#include <PCH/pch.h>

#include "spriteAnimator.h"
#include <Engine/Graphics/Texture/texture.h>

namespace Dog {

    SpriteAnimator::SpriteAnimator(int firstIndex, int lastIndex, float frameDuration)
        : m_FirstIndex(firstIndex)
        , m_LastIndex(lastIndex)
        , m_CurrentIndex(firstIndex)
        , m_FrameDuration(frameDuration)
        , m_CurrentTime(0.0f) {}

    void SpriteAnimator::SetFirstIndex(int index)
    {
        m_FirstIndex = index;
    }

    void SpriteAnimator::SetFirstIndex(Texture2D& texture, int column, int row)
    {
        m_FirstIndex = row * texture.Columns + column;
    }

    void SpriteAnimator::SetLastIndex(int index)
    {
        m_LastIndex = index;
    }

    void SpriteAnimator::SetLastIndex(Texture2D& texture, int column, int row)
    {
        m_LastIndex = row * texture.Columns + column;
    }

    void SpriteAnimator::SetFrameDuration(float duration)
    {
        m_FrameDuration = duration;
    }

    // Update the animation based on delta time
    void SpriteAnimator::Update(float deltaTime) {
        if (m_FrameDuration == 0) return;

        m_CurrentTime += deltaTime;

        // Loop the animation
        while (m_CurrentTime >= m_FrameDuration) {
            m_CurrentTime -= m_FrameDuration;
            m_CurrentIndex = (m_CurrentIndex + 1) % (m_LastIndex - m_FirstIndex + 1);
        }
    }

    // Get the current frame index
    int SpriteAnimator::GetCurrentFrameIndex() const {
        return m_FirstIndex + m_CurrentIndex;
    }

}