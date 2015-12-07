#pragma once

#include "MathFuncs.hpp"
#include "Body.hpp"
#include "Shape.hpp"
#include "Drawable.hpp"
#include "AnimatedModel.hpp"

namespace noob
{
	class actor
	{
		public:
			// TODO: Replace animated_model with drawable
			void init(noob::body* body_ptr, noob::shape* shape_ptr, noob::animated_model* drawable_ptr)
			{
				_body_ptr = body_ptr;
				_shape_ptr = shape_ptr;
				_drawable_ptr = drawable_ptr;
			}

		protected:
			noob::body* _body_ptr;
			noob::shape* _shape_ptr;
			noob::animated_model* _drawable_ptr;
	};
}
