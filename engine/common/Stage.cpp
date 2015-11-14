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
/*
	set_model_name(make_model(noob::basic_mesh::sphere(0.5)), "unit-sphere");
	set_model_name(make_model(noob::basic_mesh::cube(1.0, 1.0, 1.0)), "unit-cube");
	set_model_name(make_model(noob::basic_mesh::cylinder(0.5, 1.0)), "unit-cylinder");
	set_model_name(make_model(noob::basic_mesh::cone(0.5, 1.0)), "unit-cone");



	set_skeleton_name(make_skeleton("seymour.skel.ozz"), "human");

	set_light_name(make_light(noob::light()), "default");
	set_reflectance_name(make_reflectance(noob::reflectance()), "default");

	noob::basic_rendererr::uniform_info dbg_shader;
	dbg_shader.colour = noob::vec4(0.0, 0.3, 0.3, 1.0);	
	set_shader_name(make_shader(dbg_shader), "debug");
*/
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
	// TODO: Use frustum + physics world collisions to determine which items are visible, and then draw them.
}


/*
void noob::stage::draw(noob::drawable* d, const noob::mat4& transform) const
{
}


*/
noob::basic_model_component::handle noob::stage::add_basic_model(const noob::basic_mesh& m)
{
	return basic_models.add(std::make_unique<noob::basic_model>(m));
}


noob::animated_model_component::handle noob::stage::add_animated_model(const std::string& filename)
{
	return animated_models.add(std::make_unique<noob::animated_model>(filename));
}


noob::skeleton_component::handle noob::stage::add_skeleton(const std::string& filename)
{
	std::unique_ptr<noob::skeletal_anim> temp = std::make_unique<noob::skeletal_anim>();
	temp->init(filename);
	return skeletons.add(std::move(temp));
}

noob::actor_component::handle noob::stage::add_actor(basic_model_component::handle, skeleton_component::handle, const noob::vec3& = noob::vec3(0.0, 0.0, 0.0), const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0))
{
}


noob::prop_component::handle noob::stage::add_prop(basic_model_component::handle, const noob::vec3& = noob::vec3(0.0, 0.0, 0.0), const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0))
{
}


noob::scenery_component::handle noob::stage::add_scenery(basic_model_component::handle, const noob::vec3& = noob::vec3(0.0, 0.0, 0.0), const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0))
{

}


noob::light_component::handle noob::stage::add_light(const noob::light& arg)
{
	return lights.add(arg);
}


noob::reflection_component::handle noob::stage::add_reflection(const noob::reflection& arg)
{
	return reflections.add(arg);
}


noob::shader_component::handle noob::stage::add_shader(const noob::prepared_shaders::info& arg)
{
	return shaders.add(arg);
}


noob::shape_component::handle noob::stage::sphere(float r)
{
	auto search = spheres.find(r);
	if (search == spheres.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->sphere(r);
		spheres[r] = shapes.add(std::move(temp));
		return spheres[r];
	}
	else return spheres[r];

}


noob::shape_component::handle noob::stage::box(float x, float y, float z)
{
	auto search = boxes.find(std::make_tuple(x,y,z));
	if (search == boxes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->box(x, y, z);
		auto results = boxes.insert(std::make_pair(std::make_tuple(x,y,z), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	else return boxes[std::make_tuple(x,y,z)];
}

noob::shape_component::handle noob::stage::cylinder(float r, float h)
{
	auto search = cylinders.find(std::make_tuple(r, h));
	if (search == cylinders.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cylinder(r, h);
		auto results = cylinders.insert(std::make_pair(std::make_tuple(r, h), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	else return cylinders[std::make_tuple(r, h)];
}


noob::shape_component::handle noob::stage::cone(float r, float h)
{
	auto search = cones.find(std::make_tuple(r, h));
	if (search == cones.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cone(r, h);
		auto results = cones.insert(std::make_pair(std::make_tuple(r, h), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	else return cones[std::make_tuple(r, h)];
}


noob::shape_component::handle noob::stage::capsule(float r, float h)
{
	auto search = capsules.find(std::make_tuple(r, h));
	if (search == capsules.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->capsule(r, h);
		auto results = capsules.insert(std::make_pair(std::make_tuple(r, h), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	else return capsules[std::make_tuple(r, h)];
}


noob::shape_component::handle noob::stage::plane(const noob::vec3& normal, float offset)
{
	auto search = planes.find(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset));
	if (search == planes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->plane(normal, offset);
		auto results = planes.insert(std::make_pair(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset), shapes.add(std::move(temp))));
		return (results.first)->second;
	}
	else return planes[std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset)];
}


noob::shape_component::handle noob::stage::hull(const std::vector<noob::vec3>& points)
{
	std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
	temp->convex(points);
	return shapes.add(std::move(temp));
}


noob::shape_component::handle noob::stage::trimesh(const noob::basic_mesh& mesh)
{
	std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
	temp->trimesh(mesh);
	return shapes.add(std::move(temp));
}
