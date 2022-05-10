#include "gravity_physics_system.h"

namespace ve
{
	void GravityPhysicsSystem::update(std::vector<VeGameObject>& objs, float dt, unsigned int substeps /*= 1*/)
	{
		const float stepDelta = dt / substeps;
		for (int i = 0; i < substeps; i++) 
		{
			stepSimulation(objs, stepDelta);
		}
	}

	glm::vec2 GravityPhysicsSystem::computeForce(VeGameObject& fromObj, VeGameObject& toObj) const
	{
		auto offset = fromObj.transform2d.translation - toObj.transform2d.translation;
		float distanceSquared = glm::dot(offset, offset);

		// clown town - just going to return 0 if objects are too close together...
		if (glm::abs(distanceSquared) < 1e-10f) {
			return { .0f, .0f };
		}

		float force =
			strengthGravity * toObj.rigidBody2d.mass * fromObj.rigidBody2d.mass / distanceSquared;
		return force * offset / glm::sqrt(distanceSquared);
	}

	void GravityPhysicsSystem::stepSimulation(std::vector<VeGameObject>& physicsObjs, float dt)
	{
		// Loops through all pairs of objects and applies attractive force between them
		for (auto iterA = physicsObjs.begin(); iterA != physicsObjs.end(); ++iterA) 
		{
			auto& objA = *iterA;
			for (auto iterB = iterA; iterB != physicsObjs.end(); ++iterB) {
				if (iterA == iterB) continue;
				auto& objB = *iterB;

				auto force = computeForce(objA, objB);
				objA.rigidBody2d.velocity += dt * -force / objA.rigidBody2d.mass;
				objB.rigidBody2d.velocity += dt * force / objB.rigidBody2d.mass;
			}
		}

		// update each objects position based on its final velocity
		for (auto& obj : physicsObjs) 
		{
			obj.rigidBody2d.velocity -= dt * obj.rigidBody2d.velocity * .001f;
			obj.transform2d.translation += dt * obj.rigidBody2d.velocity;
		}
	}

}