// Kinematic, until self_control == false. The it beomes dynamic body
#pragma once


#include "MathFuncs.hpp"
#include "reactphysics3d.h"
#include "Prop.hpp"

namespace noob
{
	class character_controller
	{
		public:
			character_controller() : self_control(true) {}

			void init(rp3d::DynamicsWorld* _world, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::mat4&, float _mass, float _width, float _height, float _max_speed);

			void update();

			void step(float dt, bool forward, bool back, bool left, bool right, bool jump);
			
			noob::mat4 get_transform() const { return prop.get_transform(); }
			//void print_debug_info() const;
			noob::prop* get_prop() const { return const_cast<noob::prop*>(&prop); }

		protected:

			class raycast_callback : public rp3d::RaycastCallback
			{
				public:
					raycast_callback(noob::prop* _prop_ptr) : prop_ptr(_prop_ptr) {}
					virtual rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& info)
					{
						fmt::MemoryWriter w;
						w << "[CharacterController] raycast hit point = (" << info.worldPoint.x << ", " << info.worldPoint.y << ", " << info.worldPoint.z << ")";
						logger::log(w.str());
						return rp3d::decimal(1.0);
					}
				protected:
					noob::prop* prop_ptr;
			};

			bool self_control;
			float width, mass, height, current_speed, max_speed;
			noob::prop prop;
			rp3d::ProxyShape* proxy_shape;
			rp3d::DynamicsWorld* world;
	};
}
