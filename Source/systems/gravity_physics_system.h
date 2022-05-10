#pragma once 

#include "ve_game_object.h"

// std
#include <vector>

namespace ve
{
	class GravityPhysicsSystem
	{
	public:
		GravityPhysicsSystem(float strength) : strengthGravity{ strength } {}

		void update(std::vector<VeGameObject>& objs, float dt, unsigned int substeps = 1);
		glm::vec2 computeForce(VeGameObject& fromObj, VeGameObject& toObj) const;
	private:
		void stepSimulation(std::vector<VeGameObject>& physicsObjs, float dt);

		const float strengthGravity;
	};
}
