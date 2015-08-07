// Each actor is linked to a physics body. When the scene needs serializing, a shared_ptr to a BodySerializeHelper gets created, and each actor is linked to it via weak_ptr. The scene also creates shared_ptr's to JointSerializeHelper items, linked to their relevant actors via weak_ptr. These are used to recreate the scene when it gets loaded.
#pragma once

#include <memory>

namespace noob
{
	struct body_serialization_helper
	{
		enum body_type {OBSERVER, STATIC, KINEMATIC, DYNAMIC};
		body_type type;
		bool can_sleep;

	};


	struct joint_serialization_helper
	{
		
	};
}
