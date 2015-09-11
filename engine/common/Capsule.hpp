#include "Shape.hpp"

namespace noob
{
	namespace physics
	{
		class capsule : protected shape
		{
			public:
				template <class Archive>
					void serialize(Archive& ar)
					{
						ar(radius, height);
					}

				template <class Archive>
					static void load_and_construct( Archive & ar, cereal::construct<noob::physics::capsule> & construct )
					{
						float radius, height;
						ar(radius, height);
						construct(radius, height);
					}

				capsule(float _radius, float _height) : radius(_radius), height(_height)
			{
				inner_shape = new btCapsuleShape(radius, height);
			}
			protected:
				float radius, height;
		};
	}
}
