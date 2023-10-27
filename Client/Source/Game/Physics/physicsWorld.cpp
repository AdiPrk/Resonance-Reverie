#include <PCH/pch.h>
#include "physicsWorld.h"
#include <Source/Game/Entities/Player/player.h>

b2World physicsWorld(b2Vec2(0.0f, 40.0f)); // no gravity!

bool CheckFixtureBits(b2Fixture* f1, b2Fixture* f2, uint16 b1, uint16 b2) {
    return (f1->GetFilterData().categoryBits == b1 && f2->GetFilterData().categoryBits == b2) ||
           (f2->GetFilterData().categoryBits == b1 && f1->GetFilterData().categoryBits == b2);
}

void PhysicsContactListener::BeginContact(b2Contact* contact) {
    // handle beginning of the collision
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    // Check if Player and Enemy or Lava
    if (CheckFixtureBits(fixtureA, fixtureB, F_PLAYER, F_ENEMY)
     || CheckFixtureBits(fixtureA, fixtureB, F_PLAYER, F_LAVA)) {

        Player* player = nullptr;
        if (fixtureA->GetFilterData().categoryBits == F_PLAYER) {
            player = reinterpret_cast<Player*>(fixtureA->GetUserData().pointer);
        }
        else if (fixtureB->GetFilterData().categoryBits == F_PLAYER) {
            player = reinterpret_cast<Player*>(fixtureB->GetUserData().pointer);
        }

        if (player) {
            player->Respawn();
            contact->SetEnabled(false);
        }
    }

    // Check if player's foot sensor and ground
    
}

void PhysicsContactListener::EndContact(b2Contact* contact) {
    // handle beginning of the collision
    // b2Fixture* fixtureA = contact->GetFixtureA();
    // b2Fixture* fixtureB = contact->GetFixtureB();
}

void PhysicsContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    // handle before collision solver
}

void PhysicsContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
    // handle after collision solver
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    if ((fixtureA->GetFilterData().categoryBits == F_PLAYER && fixtureB->GetFilterData().categoryBits == F_BLOCK) ||
        (fixtureB->GetFilterData().categoryBits == F_PLAYER && fixtureA->GetFilterData().categoryBits == F_BLOCK)) {

        // Determine which fixture is which
        Player* player = nullptr;
        b2Fixture* otherFixture = nullptr;
        if (fixtureA->GetFilterData().categoryBits == F_PLAYER) {
            player = reinterpret_cast<Player*>(fixtureA->GetUserData().pointer);
            otherFixture = fixtureB;
        }
        else if (fixtureB->GetFilterData().categoryBits == F_PLAYER) {
            player = reinterpret_cast<Player*>(fixtureB->GetUserData().pointer);
            otherFixture = fixtureA;
        }

        // Set player as grounded
        if (player) {
            b2WorldManifold worldManifold;
            contact->GetWorldManifold(&worldManifold);
            b2Vec2 normal = worldManifold.normal;

            // If the player fixture is fixtureB, invert the normal
            if (fixtureB->GetFilterData().categoryBits == F_PLAYER) {
                normal = -normal;
            }

            // Check if the collision is mainly vertical and from above the player
            if (/*abs(normal.x) < abs(normal.y) &&*/ normal.y > 0) {
                player->isGrounded = true;
            }
        }
    }
}
