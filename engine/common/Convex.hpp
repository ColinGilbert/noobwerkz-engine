#pragma once


#include "Shape.hpp"
#include "MathFuncs.hpp"

namespace noob
{
	namespace physics
	{
		class convex : protected shape
		{
			public:
				template <class Archive>
					void serialize(Archive& ar)
					{
						ar(points);
					}


				template <class Archive>
					static void load_and_construct( Archive & ar, cereal::construct<noob::physics::convex> & construct )
					{
						std::vector<noob::vec3> points;
						ar(points);
						construct(points);
					}
				convex(const std::vector<noob::vec3> _points) : points(_points)
			{	
				inner_shape = new btConvexHullShape(&points[0].v[0], points.size());
				//	inner_shape = new btConvexHullShape();
				/*
				   for ( noob::vec3 p : points)
				   {
				   inner_shape.addPoint(btVector3(p.v[0], p.v[1], pv.[2]));
				   }
				   */
			}

			protected:
				std::vector<noob::vec3> points;
		};
	}
}
