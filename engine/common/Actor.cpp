#include "Actor.hpp"
//#include "TransformHelper.hpp"

void noob::actor::set_drawable(const std::shared_ptr<noob::model>& _drawable)
{
	drawable = _drawable;
}


void noob::actor::set_skeleton(const std::shared_ptr<noob::skeletal_anim>& _anim)
{
	anim = _anim;
}


void noob::actor::set_shading(const std::shared_ptr<noob::prepared_shaders::info>& _shader_info)
{
	shader_info = _shader_info;
}

void noob::actor::set_controller(rp3d::DynamicsWorld* world, const noob::mat4& transform, float mass, float width, float height, float max_speed)
{
	controller.init(world, transform, mass, width, height, max_speed);
}

void noob::actor::update(double dt, bool forward, bool backward, bool left, bool right, bool jump)
{
	// controller.step(static_cast<float>(dt), forward, backward, left, right, jump);
}


bool noob::actor::set_destination(const noob::vec3& pos)
{
	path.clear();
	path.push_back(pos);
	return true;
}

bool noob::actor::add_to_path(const std::vector<noob::vec3>& path_segment)
{
	for (noob::vec3 p : path_segment)
	{
		path.push_back(p);
	}
	return true;
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
