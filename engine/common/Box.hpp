#pragma once

#include "Shape.hpp"

namespace noob
{
	class box : protected shape
	{
		public:
			template <class Archive>
				void serialize(Archive& ar)
				{
					ar(width, height, depth);
				}

			template <class Archive>
				static void load_and_construct( Archive & ar, cereal::construct<noob::box> & construct )
				{
					float width, height, depth;
					ar(width, height, depth);
					construct(width, height, depth);
				}

			box(float _mass, float _width, float _height, float _depth) : width(_width), height(_height), depth(_depth)
		{
			inner_shape = new btBoxShape(btVector3(width, height, depth));
		}

		protected:
			float width, height, depth;
	};
}
