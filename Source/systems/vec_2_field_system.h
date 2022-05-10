#pragma once 

#include "gravity_physics_system.h"

// std
#include <vector>

namespace ve
{
	class Vec2FieldSystem
	{
	public:
		void update(
			const GravityPhysicsSystem& physicsSystem, 
			std::vector<VeGameObject>& physicsObjs, 
			std::vector<VeGameObject>& vectorField);
	};
}
