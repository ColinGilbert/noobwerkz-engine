#include "Actor.hpp"
#include "Actor.hpp"
/*
void noob::actor::init(const std::shared_ptr<noob::physics_body>& bod, const std::shared_ptr<noob::drawable3d>& drawable, const std::shared_ptr<noob::animated_model>& anim)
{

}
*/
/*
void noob::actor::set_position(const noob::vec3&)
{

}


noob::vec3 noob::actor::get_position() const
{

}


void noob::actor::set_orientation(const noob::versor&)
{

}


noob::versor noob::actor::get_orientation() const
{

}
*/

void noob::actor::set_transform(const noob::mat4&)
{

}


noob::mat4 noob::actor::get_transform()
{
	if (physics_driven)
	{
		xform = body.get_transform();
	}
	return xform;

}


void noob::actor::add_to_path(const std::vector<noob::vec3>& path_segment)
{
	//path.push_back(path_segment);
}


std::vector<noob::vec3> noob::actor::get_path() const
{
	std::vector<noob::vec3> p;
	for (noob::vec3 v : path)
	{
		p.push_back(v);
	}
	return p;

}


void noob::actor::clear_path()
{
	path.clear();
}



void noob::actor::face_point(const noob::vec3& point)
{

}


void noob::actor::draw_skeleton() const
{

}


void noob::actor::set_physics_control(bool)
{


}
