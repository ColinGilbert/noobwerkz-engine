#include "Actor.hpp"
//#include "TransformHelper.hpp"
void noob::actor::init(rp3d::DynamicsWorld* _world, const std::shared_ptr<noob::model>& _model, const std::shared_ptr<noob::skeletal_anim>& _anim, const std::shared_ptr<noob::prepared_shaders::info>& _shader_info, const noob::mat4& _mat, float _mass, float _width, float _height, float _max_speed)
 {
	// mass = _mass;
	// width = _width;
	// height = _height;
	max_speed = _max_speed;
	anim = _anim;
	prop.init(_world, _model, _shader_info, _mat, rp3d::KINEMATIC);
	rp3d::CapsuleShape capsule(_width / 2, _height);
	prop.get_body()->addCollisionShape(capsule, rp3d::Transform::identity(), 0.0);//,);// _mass);
	//destination.init()
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
