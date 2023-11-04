#include <PCH/pch.h>
#include "physicsWorld.h"
#include <Source/Game/Entities/Player/player.h>
#include <Source/Game/Entities/Interactables/grapplePoint.h>
#include <Source/Graphics/Resourcemanager/resourceManager.h>
#include <Source/Graphics/Renderer/spriteRenderer.h>

b2World physicsWorld(b2Vec2(0.0f, 20.0f));

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

    if (CheckFixtureBits(fixtureA, fixtureB, F_PLAYER, F_GRAPPLE_POINT))
    {
        Player* player = nullptr;
        GrapplePoint* point = nullptr;

        if (fixtureA->GetFilterData().categoryBits == F_PLAYER) {
            player = reinterpret_cast<Player*>(fixtureA->GetUserData().pointer);
            point = reinterpret_cast<GrapplePoint*>(fixtureB->GetUserData().pointer);
        }
        else if (fixtureB->GetFilterData().categoryBits == F_PLAYER) {
            player = reinterpret_cast<Player*>(fixtureB->GetUserData().pointer);
            point = reinterpret_cast<GrapplePoint*>(fixtureA->GetUserData().pointer);
        }

        if (player && point) {
            player->AddGrapplePoint(point->GetPosition() + point->GetSize() / 2.0f);
        }
    }

    // Check if one fixture is the player's foot sensor and the other is the ground
    if ((fixtureA->IsSensor() && fixtureB->GetFilterData().categoryBits == F_BLOCK) ||
        (fixtureB->IsSensor() && fixtureA->GetFilterData().categoryBits == F_BLOCK)) {

        // Determine which fixture is the player
        Player* player = nullptr;
        bool foot = false; // true if left
        if (fixtureA->GetFilterData().categoryBits == F_PLAYER_LEFTFOOT_SENSOR) {
            player = reinterpret_cast<Player*>(fixtureA->GetUserData().pointer);
            foot = true;
        }
        else if (fixtureB->GetFilterData().categoryBits == F_PLAYER_LEFTFOOT_SENSOR) {
            player = reinterpret_cast<Player*>(fixtureB->GetUserData().pointer);
            foot = true;
        }
        else if (fixtureA->GetFilterData().categoryBits == F_PLAYER_RIGHTFOOT_SENSOR) {
            player = reinterpret_cast<Player*>(fixtureA->GetUserData().pointer);
        }
        else if (fixtureB->GetFilterData().categoryBits == F_PLAYER_RIGHTFOOT_SENSOR) {
            player = reinterpret_cast<Player*>(fixtureB->GetUserData().pointer);
        }

        // Set player as grounded
        if (player) {
            if (foot) {
                player->leftSensorGrounded = true;
            }
            else {
                player->rightSensorGrounded = true;
            }
        }
    }
}

void PhysicsContactListener::EndContact(b2Contact* contact) {
    // handle beginning of the collision
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    // Check if one fixture is the player's foot sensor and the other is the ground
    if ((fixtureA->IsSensor() && fixtureB->GetFilterData().categoryBits == F_BLOCK) ||
        (fixtureB->IsSensor() && fixtureA->GetFilterData().categoryBits == F_BLOCK)) {

        // Determine which fixture is the player
        Player* player = nullptr;
        bool foot = false; // true if left
        if (fixtureA->GetFilterData().categoryBits == F_PLAYER_LEFTFOOT_SENSOR) {
            player = reinterpret_cast<Player*>(fixtureA->GetUserData().pointer);
            foot = true;
        }
        else if (fixtureB->GetFilterData().categoryBits == F_PLAYER_LEFTFOOT_SENSOR) {
            player = reinterpret_cast<Player*>(fixtureB->GetUserData().pointer);
            foot = true;
        }
        else if (fixtureA->GetFilterData().categoryBits == F_PLAYER_RIGHTFOOT_SENSOR) {
            player = reinterpret_cast<Player*>(fixtureA->GetUserData().pointer);
        }
        else if (fixtureB->GetFilterData().categoryBits == F_PLAYER_RIGHTFOOT_SENSOR) {
            player = reinterpret_cast<Player*>(fixtureB->GetUserData().pointer);
        }

        // Set player as grounded
        if (player) {
            if (foot) {
                player->leftSensorGrounded = false;
            }
            else {
                player->rightSensorGrounded = false;
            }
        }
    }

    if (CheckFixtureBits(fixtureA, fixtureB, F_PLAYER, F_GRAPPLE_POINT))
    {
        Player* player = nullptr;
        GrapplePoint* point = nullptr;

        if (fixtureA->GetFilterData().categoryBits == F_PLAYER) {
            player = reinterpret_cast<Player*>(fixtureA->GetUserData().pointer);
            point = reinterpret_cast<GrapplePoint*>(fixtureB->GetUserData().pointer);
        }
        else if (fixtureB->GetFilterData().categoryBits == F_PLAYER) {
            player = reinterpret_cast<Player*>(fixtureB->GetUserData().pointer);
            point = reinterpret_cast<GrapplePoint*>(fixtureA->GetUserData().pointer);
        }

        if (player && point) {
            player->RemoveGrapplePoint(point->GetPosition() + point->GetSize() / 2.0f);
        }
    }
}

void PhysicsContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    // handle before collision solver
}

void PhysicsContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
    // handle after collision solver
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    if (fixtureA->GetFilterData().categoryBits == F_PLAYER && !fixtureB->IsSensor()
     || fixtureB->GetFilterData().categoryBits == F_PLAYER && !fixtureA->IsSensor()) {

        Player* player = nullptr;
        if (fixtureA->GetFilterData().categoryBits == F_PLAYER) {
            player = reinterpret_cast<Player*>(fixtureA->GetUserData().pointer);
        }
        else if (fixtureB->GetFilterData().categoryBits == F_PLAYER) {
            player = reinterpret_cast<Player*>(fixtureB->GetUserData().pointer);
        }
        
        if (player) 
        {
            player->hitSolidObject = true;
        }
    }

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


void RenderColliders(SpriteRenderer& renderer)
{
    for (b2Body* body = physicsWorld.GetBodyList(); body != nullptr; body = body->GetNext())
    {
        float angle = body->GetAngle();

        // Check if the body is the anchor
        if (body->GetType() == b2_staticBody && body->GetFixtureList() == nullptr)
        {
            glm::vec2 anchorPosition = PhysicsUtils::MetersToPixels(body->GetPosition());
            float anchorSize = 5.0f;

            renderer.DrawSprite(ResourceManager::GetTexture("particle"), anchorPosition, glm::vec2(anchorSize), 0, { 1.0, 0.0, 0.0, 1.0f });
            continue;
        }

        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext())
        {
            b2Shape::Type shapeType = fixture->GetType();

            if (shapeType == b2Shape::e_polygon)
            {
                b2PolygonShape* polygonShape = (b2PolygonShape*)fixture->GetShape();
                int vertexCount = polygonShape->m_count;

                glm::vec2 centroid = PhysicsUtils::MetersToPixels(body->GetWorldPoint(polygonShape->m_centroid));

                float minX = FLT_MAX, minY = FLT_MAX, maxX = FLT_MIN, maxY = FLT_MIN;
                for (int i = 0; i < vertexCount; ++i)
                {
                    b2Vec2 vertex = polygonShape->m_vertices[i];
                    minX = std::min(minX, vertex.x);
                    minY = std::min(minY, vertex.y);
                    maxX = std::max(maxX, vertex.x);
                    maxY = std::max(maxY, vertex.y);
                }

                glm::vec2 size = PhysicsUtils::MetersToPixels(b2Vec2(maxX - minX, maxY - minY));
                glm::vec2 position = centroid - size / 2.f;

                uint16 bits = fixture->GetFilterData().categoryBits;

                // drawing particle cuz it has just an outline lol
                if (bits == F_PLAYER) {
                    renderer.DrawSprite(ResourceManager::GetTexture("particle"), position, size, angle, COLOR_F_PLAYER);
                }
                else if (bits == F_BLOCK) {
                    renderer.DrawSprite(ResourceManager::GetTexture("particle"), position, size, angle, COLOR_F_BLOCK);
                }
                else if (bits == F_LAVA) {
                    renderer.DrawSprite(ResourceManager::GetTexture("particle"), position, size, angle, COLOR_F_LAVA);
                }
                else {
                    // default
                    renderer.DrawSprite(ResourceManager::GetTexture("square"), position, size, angle, { 1.0, 1.0, 1.0, 1.0f });
                }
            }
            else if (shapeType == b2Shape::e_circle) {
                b2CircleShape* circleShape = (b2CircleShape*)fixture->GetShape();
                glm::vec2 position = PhysicsUtils::MetersToPixels(body->GetWorldPoint(circleShape->m_p));
                float radius = PhysicsUtils::MetersToPixels(circleShape->m_radius);
                position -= radius;
                radius *= 2;
                renderer.DrawSprite(ResourceManager::GetTexture("circleOutline"), position, { radius, radius }, angle, { 1.0, 1.0, 1.0, 1.0f });
            }
            // else, handle other types in the future eg shapeType == b2Shape::e_circle
        }
    }
}