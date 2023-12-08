#pragma once


namespace Dog {

    class Texture2D;
    
    class SpriteAnimator {
    private:
        int m_FirstIndex;      // First frame index
        int m_LastIndex;       // Last frame index
        int m_CurrentIndex;    // Current index (relative to first to last)
        float m_FrameDuration; // Duration of each frame
        float m_CurrentTime;   // Current time elapsed

    public:
        // Constructor
        SpriteAnimator() = default;
        SpriteAnimator(int firstIndex, int lastIndex, float frameDuration);

        // Set Information
        void SetFirstIndex(int index);
        void SetFirstIndex(Texture2D& texture, int column, int row);
        void SetLastIndex(int index);
        void SetLastIndex(Texture2D& texture, int column, int row);
        void SetFrameDuration(float duration);

        // Update the animation based on delta time
        void Update(float deltaTime);

        // Get the current frame index
        int GetCurrentFrameIndex() const;
    };

}