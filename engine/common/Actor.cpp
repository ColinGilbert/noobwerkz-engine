#include "Actor.hpp"
#include "TransformHelper.hpp"



void noob::actor::update()
{
	xform = body.get_transform();
}


void noob::actor::add_to_path(const std::vector<noob::vec3>& path_segment)
{
	for (noob::vec3 p : path_segment)
	{
		path.push_back(p);
	}
}


void noob::actor::clear_path()
{
	path.clear();
}


std::vector<noob::vec3> noob::actor::get_path_vector() const
{
	std::vector<noob::vec3> p;
	p.reserve(path.size());
	for (noob::vec3 v : path)
	{
		p.emplace_back(v);
	}
	return p;

}
