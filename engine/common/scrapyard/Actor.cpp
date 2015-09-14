#include "Actor.hpp"

/*
void noob::actor::init(btDynamicsWorld* _world, const std::shared_ptr<noob::prop>& _prop, const std::shared_ptr<noob::skeletal_anim>& _anim)
{
	controller.init(_world, _prop, width, height);
	anim = _anim;
}
*/
void noob::actor::init(btDynamicsWorld* _world, noob::drawable* _drawable, noob::skeletal_anim* _anim)
{
	controller.init(_world);
	drawable = _drawable;
	anim = _anim;
}


void noob::actor::update()
{
	controller.update();
}


void noob::actor::move(bool forward, bool backward, bool left, bool right, bool jump)
{
	controller.step(forward, backward, left, right, jump);
}


void noob::actor::set_destination(const noob::vec3& pos)
{
	path.clear();
	path.push_back(pos);
}


noob::vec3 noob::actor::get_destination() const
{
	if (!path.empty())
	{
		return path.front();
	}
	else return get_position();

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


std::vector<noob::vec3> noob::actor::get_path() const
{
	std::vector<noob::vec3> p;
	p.reserve(path.size());
	for (noob::vec3 v : path)
	{
		p.emplace_back(v);
	}
	return p;
}
