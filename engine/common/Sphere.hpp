#include "Shape.hpp"

namespace noob
{
	namespace physics
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
					static void load_and_construct( Archive & ar, cereal::construct<noob::physics::sphere> & construct )
					{
						float radius;
						ar(radius);
						construct(radius);
					}

				sphere(float _radius) : radius(_radius)
			{
				inner_shape = new btSphereShape(radius);
			}

			protected:
				float radius;
		};
	}
}
