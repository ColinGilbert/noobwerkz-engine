#pragma once

#include <tuple>
#include <vector>

#include "Body.hpp"
#include "Physics.hpp"
#include "Prop.hpp"
#include "CompoundShape.hpp"


namespace noob
{
	class assembly
	{

		// This wraps around a body with a compound shape. It also stores the prop handles which pertain to the (now disactivated) bodies.
		public:
			void init(noob::physics &, float mass, const noob::vec3f& pos, const noob::versorf& orient, bool ccd, const std::vector<noob::prop_handle>&, const std::vector<noob::compound_shape::child_info>&) noexcept(true);

			noob::mat4f get_child_transform(uint32_t) const noexcept(true);

			noob::body& get_body() noexcept(true)
			{
				return bod;
			}

			noob::body_handle get_body_handle() const noexcept(true)
			{
				return bod_h;
			}


		protected:
			std::vector<noob::prop_handle> props; // Indexed in the same manner
			noob::compound_shape compound;

			noob::body bod;
			noob::body_handle bod_h;
	};

	typedef noob::handle<noob::assembly> assembly_handle;
}
