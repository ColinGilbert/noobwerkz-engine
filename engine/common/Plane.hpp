#include "Shape.hpp"
#include "MathFuncs.hpp"

namespace noob
{
	namespace physics
	{
		class plane : protected shape
		{
			public:
				template <class Archive>
					void serialize(Archive& ar)
					{
						ar(normal, offset);
					}

				template <class Archive>
					static void load_and_construct( Archive & ar, cereal::construct<noob::physics::plane> & construct )
					{

						noob::vec3 normal;
						float offset;
						ar(normal, offset);
						construct(normal, offset);
					}


				plane(const noob::vec3& _normal, float _offset) : normal(_normal), offset(_offset)
			{
				inner_shape = new btStaticPlaneShape(btVector3(normal.v[0], normal.v[1], normal.v[2]), offset);
			}
			protected:
				noob::vec3 normal;
				float offset;
		};
	}
}
