#include "../include/physics.hpp"
#include <algorithm>
#include "../include/common.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

// logic for gravity and potentially friction calculations go here

Physics::Physics() = default;

Physics::~Physics() = default;

int floor_tolerance = 40;

bool circleToCircleIntersection(vec2 c1, vec2 c2, float r1, float r2)
{
    float xDistance = c1.x - c2.x;
    float yDistance = c1.y - c2.y;
    float radius = r1 + r2;
    radius *= 0.45f;
    return xDistance * xDistance + yDistance * yDistance < radius * radius;
}

float find_length(vec2 v)
{
    return sqrt((v.x * v.x) + (v.y * v.y));
}

vec2 normalize_length(vec2 v)
{
    float length = find_length(v);
    v = {v.x / length, v.y / length};
    return v;
}


Physics::CollisionNode Physics::collideWithEnemy (Player *p, const Enemy *e) {

    float other_r = std::max(e->get_bounding_box().x, e->get_bounding_box().y);
    float my_r = std::max(p->width, p->height);

    bool isCollided = circleToCircleIntersection(p->get_position(), e->get_position(), other_r, my_r);

    Physics::CollisionNode collisionNode{};
    collisionNode.isCollided = isCollided;
    collisionNode.angleOfCollision = 0;
    return collisionNode;
}

Physics::CollisionNode Physics::collisionWithFixedWalls(Player *p, const Floor *f) {
    float other_r = std::max(p->get_bounding_box().x, f->get_bounding_box().y);
    float my_r = std::max(p->width, p->height);

    bool isCollided = circleToCircleIntersection(p->get_position(), f->get_position(), other_r, my_r);

    CollisionNode collisionNode{};
    collisionNode.isCollided = isCollided;

	if (isCollided) {
		float dy = p->get_position().y - f->get_position().y;
		float dx = f->get_position().x - p->get_position().x;
		float collisionAngle = atan2(dy, dx);
        collisionNode.angleOfCollision = collisionAngle;

    } else {
        collisionNode.angleOfCollision = 0;
    }
    return collisionNode;
}

Physics::CollisionNode Physics::collideWithExit (Player *p, const Exit *e) {
	float other_r = std::max(p->get_bounding_box().x, e->get_bounding_box().y);
	float my_r = std::max(p->width, p->height);

	bool isCollided = circleToCircleIntersection(p->get_position(), e->get_position(), other_r, my_r);

    Physics::CollisionNode collisionNode{};
    collisionNode.isCollided = isCollided;
    collisionNode.angleOfCollision = 0;
    return collisionNode;
}

void Physics::characterCollisionsWithFixedComponents(Player* c, std::vector<Floor> fixedComponents) {
	bool isOnAtLeastOnePlatform = false;
	bool isLeftOfAtLeastOnePlatform = false;
	bool isRightOfAtLeastOnePlatform = false;
	bool isBelowAtLeastOnePlatform = false;

	Physics::CollisionNode collisionNode;
	for (const auto &floor : fixedComponents) {
		collisionNode = collisionWithFixedWalls(c, &floor);
		if (collisionNode.isCollided) {
			float collisionAngle = collisionNode.angleOfCollision;
			if (collisionAngle > -3 * M_PI / 4 && collisionAngle < -M_PI / 4) {
				c->set_on_platform();
				isOnAtLeastOnePlatform = true;
			}

			if (collisionAngle > -M_PI / 4 && collisionAngle < M_PI / 4) {
				isLeftOfAtLeastOnePlatform = true;
			}
			if (collisionAngle > M_PI / 4 && collisionAngle < 3 * M_PI / 4) {
				isBelowAtLeastOnePlatform = true;
			}
			if (collisionAngle > 3 * M_PI / 4 || collisionAngle < -3 * M_PI / 4) {
				isRightOfAtLeastOnePlatform = true;
			}

			// get the normalized vector
			vec2 colNormal = normalize_length(
					{floor.get_position().x - c->get_position().x, floor.get_position().y - c->get_position().y});
			vec2 newPos = {c->get_position().x - colNormal.x, c->get_position().y - colNormal.y};

			// get the floor position
			vec2 floorPos = floor.get_position();
			vec2 playPos = c->get_position();

			// if the player position deviates too much from the floor position, push the player back up
			if (floorPos.y - playPos.y < floor_tolerance) {
                c->set_position(newPos);
            }
		}
	}

	if (!isOnAtLeastOnePlatform) c->set_in_free_fall();
	c->isLeftOfPlatform = isLeftOfAtLeastOnePlatform;
	c->isRightOfPlatform = isRightOfAtLeastOnePlatform;
	c->isBelowPlatform = isBelowAtLeastOnePlatform;

}

void Physics::characterVelocityUpdate(Player* c)
{
	float platformDrag = 0.75; //eventually make this a property of the platform

	vec2 cAcc = c->get_acceleration();
	vec2 cVelocity = c->get_velocity();
	float maxVelocity = c->maxVelocity;

	cVelocity.x += cAcc.x;
	cVelocity.y += cAcc.y;

	if (cVelocity.x > maxVelocity) cVelocity.x = maxVelocity;
	if (cVelocity.x < -maxVelocity) cVelocity.x = -maxVelocity;

	if (cAcc.x < g_tolerance && cAcc.x > -g_tolerance && c->isOnPlatform)
		cVelocity.x *= platformDrag;

	if (c->isOnPlatform) {
		cVelocity.y = std::min(0.f, cVelocity.y);
	}
	if (c->isBelowPlatform) {
		cVelocity.y = std::max(0.f, cVelocity.y);
	}
	if (c->isLeftOfPlatform) {
		cVelocity.x = std::min(0.f, cVelocity.x);
	}
	if (c->isRightOfPlatform) {
		cVelocity.x = std::max(0.f, cVelocity.x);
	}

	c->set_velocity(cVelocity);
}

void Physics::characterAccelerationUpdate(Player * c)
{
	float vAcc;
	float hAcc;
	Direction h_direction = c->get_h_direction();
	float accStep = c->accStep;

	if (c->is_alive()) {
		switch (h_direction) {
			case Direction::left: hAcc = -accStep; break;
			case Direction::right: hAcc = accStep; break;
			default: hAcc = 0.f; break;
		}
		vAcc = gravityAcc;
		c->set_acceleration({ hAcc, vAcc });
	}
}

void Physics::characterRotationUpdate(Player *c, float rotation)
{

    vec2 cAcc = c->get_acceleration();
    vec2 pos = c->get_position();
    vec2 cVel = c->get_velocity();

    cVel.x += cAcc.x;
    cVel.y += cAcc.y;

//    if ((rotation > M_PI/10 && rotation < 4.72) && c->isOnPlatform) {
//        // make the player fall down
//
//
//    } else if (rotation > 4.72) {
//
//
//        c->set_velocity(cVel);
//
//
//    }
}

