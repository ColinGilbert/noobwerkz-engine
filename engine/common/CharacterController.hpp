// Kinematic, until self_control == false. Then it beomes dynamic body
#pragma once


#include "MathFuncs.hpp"
#include "Prop.hpp"
#include "reactphysics3d.h"
#include <memory>

namespace noob
{
	class character_controller
	{
		public:
			character_controller() : self_control(false), slope(0.0, 0.0, 0.0), world(nullptr) {}

			void init(rp3d::DynamicsWorld*, const std::shared_ptr<noob::prop>&, float height, float width);
			void update();
			void step(bool forward, bool back, bool left, bool right, bool jump);
			void stop();
			noob::mat4 get_transform() const { return prop->get_transform(); }
			noob::vec3 get_position() const { return prop->get_position(); }
			noob::versor get_orientation() const { return prop->get_orientation(); }
			std::string get_debug_info() const { return prop->get_debug_info(); }
			noob::prop* get_prop() const { return prop.get(); }

		protected:

	class groundcast_callback : public rp3d::RaycastCallback
		{
			public:
				groundcast_callback(noob::vec3 _from, noob::vec3 _to) : grounded(false), from(_from), to(_to), slope(0.0, 0.0, 0.0) {}
				virtual rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& info);
				bool is_grounded() const { return grounded; }
				noob::vec3 get_slope() const { return slope; }

			protected:

				//float cast_distance;
				bool grounded;
				noob::vec3 from, to, slope;				
		};

			bool self_control;
			float width, mass, height;
			noob::vec3 slope;
			std::shared_ptr<noob::prop> prop;
			rp3d::DynamicsWorld* world;
	};
}
