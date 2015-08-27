#include "Actor.hpp"
//#include "TransformHelper.hpp"
void noob::actor::init(rp3d::DynamicsWorld* _world, const std::shared_ptr<noob::model>& _model, const std::shared_ptr<noob::skeletal_anim>& _anim, const std::shared_ptr<noob::prepared_shaders::info>& _shader_info, const noob::mat4& _mat, float _mass, float _width, float _height, float _max_speed)
 {
	max_speed = _max_speed;
	world = _world;
	anim = _anim;
	controller.init( world, _model, _shader_info, _mat, _mass, _width, _height, _max_speed);
	//controller.get_prop().body->setType(rp3d::KINEMATIC);
	//controller.get_prop().add_capsule(_width, _height, mass);
}

void noob::actor::update(double dt, bool forward, bool backward, bool left, bool right, bool jump)
{
	noob::transform_helper t;
	t.translate(path.front());
	destination_prop.set_transform(t.get_matrix());// controller.step(static_cast<float>(dt), forward, backward, left, right, jump);
}


bool noob::actor::set_destination(const noob::vec3& pos)
{
	path.clear();
	path.push_back(pos);
	return true;
}


noob::mat4 noob::actor::get_transform() const
{
	return controller.get_prop().get_transform();
	
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
