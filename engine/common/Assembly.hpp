#pragma once

#include <tuple>
#include <vector>

#include "Body.hpp"
#include "Prop.hpp"
#include "CompoundShape.hpp"


namespace noob
{
	class assembly
	{
		
		// This wraps around a body with a compound shape. It also stores the prop handles which pertain to the (now disactivated) bodies.
		public:
			void init(btDynamicsWorld* const, float mass, const noob::vec3f& pos, const noob::versorf& orient, bool ccd, const std::vector<noob::prop_handle>&, const std::vector<noob::compound_shape::child_info>&) noexcept(true);
			
		protected:
			std::vector<noob::prop_handle> props; // Indexed in the same manner
			noob::body bod;
			noob::compound_shape compound;
	
	};

	typedef noob::handle<noob::assembly> assembly_handle;
}
