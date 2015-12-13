#pragma once

#include "Body.hpp"
#include "Model.hpp"

namespace noob
{
	class actor
	{
		public:
			void init(noob::body* body_ptr, noob::model* drawable_ptr)
			{
				_body_ptr = body_ptr;
				_drawable_ptr = drawable_ptr;
			}
			
			noob::model* get_drawable() const
			{
				return _drawable_ptr;
			}

		protected:
			noob::body* _body_ptr;
			noob::model* _drawable_ptr;
	};
}
