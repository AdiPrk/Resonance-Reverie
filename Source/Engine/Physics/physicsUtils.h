#pragma once

#include <Engine/PCH/pch.h>

namespace Dog {
	namespace Utils {

        constexpr float PIXELS_PER_METER = 50.0f;
        constexpr float INV_PIXELS_PER_METER = 1 / PIXELS_PER_METER;

        inline float PixelsToMeters(float pixels) {
            return pixels * INV_PIXELS_PER_METER;
        }

        inline float MetersToPixels(float meters) {
            return meters * PIXELS_PER_METER;
        }

        inline b2Vec2 PixelsToMeters(const glm::vec2& pixelsVec) {
            return b2Vec2(PixelsToMeters(pixelsVec.x), PixelsToMeters(pixelsVec.y));
        }

        inline glm::vec2 MetersToPixels(const b2Vec2& metersVec) {
            return glm::vec2(MetersToPixels(metersVec.x), MetersToPixels(metersVec.y));
        }

	}
}