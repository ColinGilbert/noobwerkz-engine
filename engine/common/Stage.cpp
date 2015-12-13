#include "Stage.hpp"

bool noob::stage::init()
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));
	
	renderer.init();
	
	auto s (pool.register_component<noob::shape>("shape"));
	shape_component = s;
	
	auto b (pool.register_component<noob::body>("body"));
	body_component = b;

	auto bm (pool.register_component<std::shared_ptr<noob::basic_model>>("basic-model"));
	basic_model_component = bm;

	auto am (pool.register_component<std::shared_ptr<noob::animated_model>>("animated-model"));
	animated_model_component = am;

	auto sa (pool.register_component<std::shared_ptr<noob::skeletal_anim>>("skeletal-anim"));
	skeletal_anim_component = sa;

	auto bsu (pool.register_component<noob::basic_renderer::uniform_info>("basic-renderer-uniform"));
	basic_shader_component = bsu;

	auto tsu (pool.register_component<noob::triplanar_gradient_map_renderer::uniform_info>("triplanar-renderer-uniform"));
	triplanar_shader_component = tsu;	


	// TODO: Add stage default components
	// noob::basic_renderer::uniform_info basic_shader_info;
	
	// basic_shader_info.colour = noob::vec4(1.0, 0.0, 0.0, 1.0);
	
	// auto s = shaders.add(basic_shader_info);
	// shaders.set_name(s, "debug");

	logger::log("[Stage] init complete.");
	return true;
}


void noob::stage::tear_down()
{
	delete dynamics_world;
	delete solver;
	delete collision_configuration;
	delete collision_dispatcher;
	delete broadphase;
}


void noob::stage::update(double dt)
{
	dynamics_world->stepSimulation(1.0/60.0, 10);
}


void noob::stage::draw() const
{
	// TODO: Use culling to determine which items are visible, and then draw them.

}


/*
std::shared_ptr<noob::basic_model> noob::stage::basic_model(const noob::basic_mesh& m)
{
	// return basic_models.add(std::make_unique<noob::basic_model>(m));
}


std::shared_ptr<noob::basic_model> noob::stage::basic_model(const std::shared_ptr<noob::shape>& input_shape)
{

}


std::shared_ptr<noob::animated_model> noob::stage::animated_model(const std::string& filename, const std::string& friendly_name)
{
	// return animated_models.add(std::make_unique<noob::animated_model>(filename));
}


std::shared_ptr<noob::skeletal_anim> noob::stage::skeleton(const std::string& filename, const std::string& friendly_name)
{
	// std::shared_ptr<noob::skeletal_anim> temp = std::make_shared<noob::skeletal_anim>();
	// temp->init(filename);
	// return temp;
}


noob::actor noob::stage::actor(const noob::prop& _prop, const std::shared_ptr<noob::animated_model>& _skeleton, const std::string& friendly_name)
{

}


noob::prop noob::stage::prop(const noob::body& _body, const std::shared_ptr<noob::basic_model>& _model, const std::string& friendly_name)
{

}


noob::scenery noob::stage::scenery(const std::shared_ptr<noob::basic_model>&, const noob::vec3& pos, const noob::versor& orient, const std::string& friendly_name)
{

}


noob::body noob::stage::body(const std::shared_ptr<noob::shape>&, float mass, const noob::vec3& pos, const noob::versor& orient)
{

}


std::shared_ptr<noob::light> noob::stage::light(const noob::light& arg)
{
	// return lights.add(arg);
}


std::shared_ptr<noob::reflection> noob::stage::reflection(const noob::reflection& arg)
{
	// return reflections.add(arg);
}


std::shared_ptr<noob::shader> noob::stage::shader(const noob::prepared_shaders::info& arg, const std::string& name)
{
	// noob::shader h;
	// if (shaders.name_exists(name))
	// {
	//	h = shaders.add(arg);
	//	shaders.set_name(h, name);
	// }
	// return h;

}


std::shared_ptr<noob::shape> noob::stage::sphere(float r)
{
	auto search = spheres.find(r);
	if (search == spheres.end())
	{
		//std::shared_ptr<noob::shape> temp = std::make_shared<noob::shape>();
		//temp->sphere(r/2.0);
		//spheres[r] = temp;
		//return spheres[r];
		spheres[r] = std::make_shared<noob::shape>();
		auto temp = spheres[r];
		temp->sphere(r);
		return temp;

	}
	return spheres[r];
}


std::shared_ptr<noob::shape> noob::stage::box(float x, float y, float z)
{
	auto search = boxes.find(std::make_tuple(x,y,z));
	if (search == boxes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->box(x, y, z);
		auto results = boxes.insert(std::make_pair(std::make_tuple(x,y,z), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return boxes[std::make_tuple(x,y,z)];
}


std::shared_ptr<noob::shape> noob::stage::cylinder(float r, float h)
{
	auto search = cylinders.find(std::make_tuple(r, h));
	if (search == cylinders.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cylinder(r, h);
		auto results = cylinders.insert(std::make_pair(std::make_tuple(r, h), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return cylinders[std::make_tuple(r, h)];
}


std::shared_ptr<noob::shape> noob::stage::cone(float r, float h)
{
	auto search = cones.find(std::make_tuple(r, h));
	if (search == cones.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cone(r, h);
		auto results = cones.insert(std::make_pair(std::make_tuple(r, h), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return cones[std::make_tuple(r, h)];
}


std::shared_ptr<noob::shape> noob::stage::capsule(float r, float h)
{
	auto search = capsules.find(std::make_tuple(r, h));
	if (search == capsules.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->capsule(r, h);
		auto results = capsules.insert(std::make_pair(std::make_tuple(r, h), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return capsules[std::make_tuple(r, h)];
}


std::shared_ptr<noob::shape> noob::stage::plane(const noob::vec3& normal, float offset)
{
	auto search = planes.find(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset));
	if (search == planes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->plane(normal, offset);
		auto results = planes.insert(std::make_pair(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	return planes[std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset)];
}


std::shared_ptr<noob::shape> noob::stage::hull(const std::vector<noob::vec3>& points, const std::string& name)
{
	noob::shape h;
	if (shapes.name_exists(name))
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->convex(points);
		h = shapes.add(std::move(temp));
		shapes.set_name(h,name);
	}
	return h;
}


std::shared_ptr<noob::shape> noob::stage::trimesh(const noob::basic_mesh& mesh, const std::string& name)
{
	noob::shape h;
	if (shapes.name_exists(name))
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->trimesh(mesh);
		h = shapes.add(std::move(temp));
		shapes.set_name(h, name);
	}
	return h;
}

*/
