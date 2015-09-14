#pragma once

#include "Shape.hpp"

namespace noob
{
	class cylinder : protected shape
	{
		public:
			template <class Archive>
				void serialize(Archive& ar)
				{
					ar(radius, height);
				}

			template <class Archive>
				static void load_and_construct( Archive & ar, cereal::construct<noob::cylinder> & construct )
				{
					float radius, height;
					ar(radius, height);
					construct(radius, height);
				}

			cylinder(float _mass, float _radius, float _height) : radius(_radius), height(_height)
		{
			inner_shape = new btCylinderShape(btVector3(radius, height/2.0, radius));
		}
		protected:
			float radius, height;
	};
}
