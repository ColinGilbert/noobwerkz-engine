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
			character_controller() : resting(false), self_control(true), time_on_ground(0.0), slope(0.0, 0.0, 0.0) {}

			void init(rp3d::DynamicsWorld* _world, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::mat4&, float _mass, float _width, float _height, float _max_speed);



			void step(float dt, bool forward, bool back, bool left, bool right, bool jump);

			noob::mat4 get_transform() const { return prop.get_transform(); }
			//void print_debug_info() const;
			noob::prop* get_prop() const { return const_cast<noob::prop*>(&prop); }

		protected:

			class groundcast_callback : public rp3d::RaycastCallback
		{
			public:
				groundcast_callback(float prop_height, noob::prop* _prop_ptr, noob::vec3 _from, noob::vec3 _to) : ideal_distance((prop_height / 2.0) + 0.5), airborne(true), prop_ptr(_prop_ptr), from(_from), to(_to), slope(0.0, 0.0, 0.0) {}
				virtual rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& info);
				bool is_grounded() const { return !airborne; }
				noob::vec3 get_slope() const { return slope; }

			protected:

				float ideal_distance;
				bool airborne;
				noob::prop* prop_ptr;
				noob::vec3 from, to, slope;				
		};
			void update(float dt);
			bool resting, self_control;
			float width, mass, height, current_speed, max_speed, time_on_ground;
			noob::vec3 slope;
			noob::prop prop;
			rp3d::ProxyShape* proxy_shape;
			rp3d::DynamicsWorld* world;
	};
}
