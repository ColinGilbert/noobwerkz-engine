// This class works with 

#pragma once

#include <algorithm>
#include <vector>
#include <array>

#include "MathFuncs.hpp"
#include "Plane.hpp"
#include "BasicMesh.hpp"

#include <lemon/list_graph.h>

namespace noob
{

	struct point
	{
		noob::vec3 position, normal;
	};

	struct poly
	{
		std::vector<noob::point> verts;
		noob::plane plane;		

		void init(const std::vector<noob::point> & list)
		{
			verts = list;
			plane.through(verts[0].position, verts[1].position, verts[2].position);
		}

		void flip()
		{
			std::reverse(verts.begin(), verts.end());
			for (size_t i = 0; i < verts.size(); i++)
			{
				verts[i].normal = negate(verts[i].normal);
			}
			plane.flip();
		}
	};


	static noob::point interpolate(const noob::point& a, const noob::point& b, float t)
	{
		noob::point ret;
		ret.position = noob::lerp(a.position, b.position, t);
		ret.normal = noob::lerp(a.normal, b.normal, t);
		return ret;
	}

	// Data type that conveniently holds and names the various places polygons belong in after being split by a plane. Far more civilized than returning those by argument.
	struct poly_split_results
	{
		std::vector<noob::poly> coplanar_front, coplanar_back, front, back;
	};

	enum class poly_type : char
	{
		COPLANAR = 0,
		FRONT = 1,
		BACK = 2,
		SPANNING = 3
	};


	static noob::poly_split_results split_poly(const noob::poly& poly_arg, const noob::plane& plane_arg)
	{
		noob::poly_type current_poly_type = noob::poly_type::COPLANAR;

		std::vector<noob::poly_type> types;

		for (size_t i = 0; i < poly_arg.verts.size(); i++) 
		{
			float t = noob::dot(plane_arg.normal, poly_arg.verts[i].position) - plane_arg.w;
			noob::poly_type temp = (t < -NOOB_EPSILON) ? noob::poly_type::BACK : ((t > NOOB_EPSILON) ? noob::poly_type::FRONT : noob::poly_type::COPLANAR);
			current_poly_type = static_cast<noob::poly_type>(static_cast<char>(current_poly_type) | static_cast<char>(temp));
			types.push_back(temp);
		}

		noob::poly_split_results results;

		// Put the polygon in the correct list, splitting it when necessary.
		switch (current_poly_type) 
		{
			case (noob::poly_type::COPLANAR):
				{
					if (noob::dot(plane_arg.normal, poly_arg.plane.normal) > 0.0)
					{
						results.coplanar_front.push_back(poly_arg);
					}
					else 
					{
						results.coplanar_back.push_back(poly_arg);
						break;
					}
				}
			case (noob::poly_type::FRONT):
				{
					results.front.push_back(poly_arg);
					break;
				}
			case (noob::poly_type::BACK):
				{
					results.back.push_back(poly_arg);
					break;
				}
			case (noob::poly_type::SPANNING):
				{
					std::vector<noob::point> f, b;
					
					for (size_t i = 0; i < poly_arg.verts.size(); i++) 
					{
						int j = (i + 1) % poly_arg.verts.size();
						noob::poly_type ti = types[i];
						noob::poly_type tj = types[j];
						noob::point vi = poly_arg.verts[i];
						noob::point vj = poly_arg.verts[j];
						if (ti != noob::poly_type::BACK)
						{
							f.push_back(vi);
						}
						if (ti != noob::poly_type::FRONT)
						{
							b.push_back(vi);
						}
						if (static_cast<noob::poly_type>(static_cast<char>(ti) | static_cast<char>(tj)) == noob::poly_type::SPANNING) 
						{
							float t = (plane_arg.w - noob::dot(plane_arg.normal, vi.position)) / noob::dot(plane_arg.normal, vj.position - vi.normal);
							noob::point v = interpolate(vi, vj, t);
							f.push_back(v);
							b.push_back(v);
						}
					}

					if (f.size() >= 3)
					{
						noob::poly p;
						p.init(f);
						results.front.push_back(p);
					}
					if (b.size() >= 3)
					{
						noob::poly p;
						p.init(f);
						results.back.push_back(p);
					}
					break;
				}
		}
		return results;
	}


	class csg
	{
		public:
			void init();
			void init(const noob::csg&);
			void init(const std::vector<noob::poly>&);
			void init(const noob::basic_mesh&);
			
			noob::basic_mesh to_basic_mesh() const;

			noob::csg clone() const;
			// Removes all polys in "other" from current csg
			void clip_to(const noob::csg& other);
			void invert();
			// Removes all polys in "list" from current csg and returns the list of removed polys
			std::vector<noob::poly> clip_polygons(const std::vector<noob::poly>& list) const;
			// Returns all polys from current tree.
			std::vector<noob::poly> all_polygons() const;
			
		protected:
			lemon::SmartDigraph graph;
			lemon::SmartDigraph::NodeMap<bool> leaf_map;
			lemon::SmartDigraph::NodeMap<std::vector<noob::poly>> poly_map;
			lemon::SmartDigraph::NodeMap<std::vector<noob::plane>> plane_map;

	};
}
