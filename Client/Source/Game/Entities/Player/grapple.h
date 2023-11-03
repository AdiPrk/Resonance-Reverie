#include <PCH/pch.h>
#include <Source/Game/Physics/physicsWorld.h>
#include <Source/Game/Inputs/inputManager.h>

enum GrapplingStatus {
    STARTED = 0,
    GRAPPLING = 1,
    FAILED = 2,
};

class SpiritGrapple {
private:
    b2DistanceJoint* grappleJoint = nullptr;
    b2Body* grapplePointBody = nullptr; // Temporary body to represent the grapple point
    bool m_Releasing = false;

public:
    float GetLength() { 
        if (!grappleJoint) return 0.0f;
        return grappleJoint->GetCurrentLength(); 
    }
    b2DistanceJoint* GetJoint() { return grappleJoint; }
    const bool GetReleasing() const { return m_Releasing; }
    void SetReleasing(bool releasing) { m_Releasing = releasing; }

    GrapplingStatus TryStartGrapple(b2Body* playerBody, glm::vec2 playerPos, glm::vec2 grappleTo) {
        if (grappleJoint) {
            return GrapplingStatus::GRAPPLING;
        }

        // Creating a temporary static body at the grapple position
        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody;
        bodyDef.position.Set(PhysicsUtils::PixelsToMeters(grappleTo.x), PhysicsUtils::PixelsToMeters(grappleTo.y));
        grapplePointBody = playerBody->GetWorld()->CreateBody(&bodyDef);

        // Initializing the distance joint
        b2DistanceJointDef jointDef;
        jointDef.Initialize(playerBody, grapplePointBody, playerBody->GetWorldCenter(), grapplePointBody->GetWorldCenter());

        float dist = PhysicsUtils::PixelsToMeters(glm::distance(playerPos, grappleTo));
        jointDef.length = dist;
        jointDef.maxLength = dist;
        jointDef.minLength = 0;

        grappleJoint = (b2DistanceJoint*)physicsWorld.CreateJoint(&jointDef);

        return GrapplingStatus::STARTED;
    }

    void UpdateGrapple(float dt) {
        if (!grappleJoint) {
            return;
        }

        if (!m_Releasing) {
            float len = grappleJoint->GetCurrentLength();

            len -= dt;

            grappleJoint->SetMaxLength(len);
            grappleJoint->SetMinLength(len);
        }

        /*if (InputManager::GetKeyDown(GLFW_KEY_UP)) {
            float len = grappleJoint->GetCurrentLength();

            len -= 0.1f;

            grappleJoint->SetMaxLength(len);
            grappleJoint->SetMinLength(len);
        }
        if (InputManager::GetKeyDown(GLFW_KEY_DOWN)) {
            float len = grappleJoint->GetCurrentLength();

            len += 0.12f;

            grappleJoint->SetMaxLength(len);
            grappleJoint->SetMinLength(len);
        }*/
    }

    void ReleaseGrapple() {
        if (grappleJoint != nullptr) {
            physicsWorld.DestroyJoint(grappleJoint);
            grappleJoint = nullptr;
        }

        if (grapplePointBody != nullptr) {
            physicsWorld.DestroyBody(grapplePointBody);
            grapplePointBody = nullptr;
        }
        
        m_Releasing = false;
    }
};
