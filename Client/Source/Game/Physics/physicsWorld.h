#pragma once

#include <PCH/pch.h>

#define DEBUG_DRAW_COLLIDERS 1

extern b2World physicsWorld;

// Fixtures
const uint16 F_PLAYER =                  1 << 0;
const uint16 F_ENEMY =                   1 << 1;
const uint16 F_BLOCK =                   1 << 2; 
const uint16 F_LAVA =                    1 << 3;
const uint16 F_SAFEZONE =                1 << 4;
const uint16 F_PLAYER_LEFTFOOT_SENSOR =  1 << 5;
const uint16 F_PLAYER_RIGHTFOOT_SENSOR = 1 << 6;
const uint16 F_GRAPPLE_POINT =           1 << 7;

const glm::vec4 COLOR_F_PLAYER(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 COLOR_F_ENEMY(0.5f, 0.5f, 0.5f, 1.0f);
const glm::vec4 COLOR_F_BLOCK(0.8f, 0.8f, 0.7f, 1.f);
const glm::vec4 COLOR_F_LAVA(0.8f, 0.3f, 0.3f, 1.0f);
const glm::vec4 COLOR_F_SAFEZONE(0.2f, 0.6f, 0.2f, 0.3f);
const glm::vec3 COLOR_F_PLAYER_FOOT_SENSOR(0.0f, 1.0f, 0.0f);

class PhysicsContactListener : public b2ContactListener {
    void BeginContact(b2Contact* contact) override;

    void EndContact(b2Contact* contact) override;

    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
};

namespace PhysicsUtils {

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

