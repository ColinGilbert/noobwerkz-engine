#include "Actor.hpp"
#include "Actor.hpp"


void noob::actor::add_to_path(const std::vector<noob::vec3>& path_segment, float speed)
{
	path.push_back(std::make_tuple(path_segment,speed));
}


void noob::actor::clear_path()
{
	
}


void noob::actor::face_point(const noob::vec3& point)
{

}


void noob::actor::stop_moving()
{
	moving = false;
}


void noob::actor::start_moving()
{
	moving = true;
}
