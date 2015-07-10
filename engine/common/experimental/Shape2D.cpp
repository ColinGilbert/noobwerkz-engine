#include "Shape2D.hpp"

//#include "clipper.hpp"
//#include "poly2tri.h"
#include "clip2tri.h"


#include "Logger.hpp"

/*
static c2t::Point vec2_to_c2t_point(const noob::vec2& v)
{
	static c2t::Point p;
	p.x = v[0];
	p.y = v[1];
	return p;
}

static noob::vec2 c2t_point_to_vec2(const c2t::Point p)
{
	static noob::vec2 v;
	v[0] = p.x;
	v[1] = p.y;
	return v;
}

void noob::shape2d::init(const std::vector<noob::vec2>& outer_boundary, const std::vector<std::vector<noob::vec2>> inner_shapes)
{
	triangles.clear();
	std::vector<std::vector<c2t::Point>> inner_polys;
	std::vector<c2t::Point> output_tris;
	std::vector<c2t::Point> bounding_poly;
	
	for (noob::vec2 v : outer_boundary)
	{
		bounding_poly.push_back(vec2_to_c2t_point(v));
	}

	for (std::vector<noob::vec2> vv : inner_shapes)
	{
		std::vector<c2t::Point> pv;
		
		for (noob::vec2 v: vv)
		{
			pv.push_back(vec2_to_c2t_point(v));
		}

		inner_polys.push_back(pv);
	}

	logger::log("shape2d - triangles copied");
	c2t::clip2tri clip2tri;
	logger::log("shape2d - clip2tri created");
	clip2tri.triangulate(inner_polys, output_tris, bounding_poly);
	logger::log("shape2d - triangulated");

	for (c2t::Point p : output_tris)
	{
		noob::vec2 v;
		v[0] = p.x;
		v[1] = p.y;
		triangles.push_back(v);
	}
}


bool noob::shape2d::valid() const
{
	return (triangles.size() > 0);
}


std::vector<noob::vec2> noob::shape2d::get_triangles() const
{
	std::vector<noob::vec2> tris;
	std::copy(triangles.begin(), triangles.end(), std::back_inserter(tris));
	return tris;
}
*/
