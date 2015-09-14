#pragma once

#include "Shape.hpp"

namespace noob
{
	class sphere : protected shape
	{
		public:
			template <class Archive>
				void serialize(Archive& ar)
				{
					ar(radius);
				}

			template <class Archive>
				static void load_and_construct( Archive & ar, cereal::construct<noob::sphere> & construct )
				{
					float radius;
					ar(radius);
					construct(radius);
				}

			sphere(float _mass, float _radius) : radius(_radius)
		{
			inner_shape = new btSphereShape(radius);
		}

		protected:
			float radius;
	};
}
