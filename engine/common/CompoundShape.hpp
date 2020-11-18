#pragma once

#include <rdestl/hash_map.h>

#include "Shape.hpp"

namespace noob
{
	// This one is kept separate, due to the different manners in which compound shapes are drawn (the drawing code indexes the other shapes and draws *them* instead)
	class compound_shape
	{
		friend class body;
		friend class ghost;

		public:
			struct child_info
			{
				noob::shape_handle shape;
				noob::vec3f pos;
				noob::versorf orient;
				float mass;
			};

			void init(const std::vector<noob::compound_shape::child_info> & arg) noexcept(true);
			
			void set_self_index(uint32_t) noexcept(true);
			uint32_t get_self_index() const noexcept(true);
			
			const btCompoundShape* get_inner() { return inner; }


		protected:
			btCompoundShape * inner;
			btVector3 inertia = btVector3(0.0, 0.0, 0.0);
			bool physics_valid = false; 
	};

	typedef noob::handle<noob::compound_shape> compound_shape_handle;
}
