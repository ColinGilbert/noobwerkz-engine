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

	auto temp_path (pool.register_component<std::vector<noob::vec3>>("path"));
	path_tag.inner = temp_path;

	auto temp_shape (pool.register_component<noob::shapes::handle>("shape"));
	shape_tag.inner = temp_shape;

	auto temp_shape_type (pool.register_component<noob::shape::type>("shape-type"));
	shape_type_tag.inner = temp_shape_type;

	auto temp_body (pool.register_component<std::shared_ptr<noob::body>>("body"));
	body_tag.inner = temp_body;

	auto temp_movement_controller (pool.register_component<noob::character_controller>("movement-controller"));
	movement_controller_tag.inner = temp_movement_controller;

	auto temp_basic_model (pool.register_component<noob::basic_models::handle>("basic-model"));
	basic_model_tag.inner = temp_basic_model;

	auto temp_animated_model (pool.register_component<noob::animated_models::handle>("animated-model"));
	animated_model_tag.inner = temp_animated_model;

	auto temp_skeletal_anim (pool.register_component<noob::skeletal_anims::handle>("skeletal-anim"));
	skeletal_anim_tag.inner = temp_skeletal_anim;

	auto temp_basic_shader (pool.register_component<noob::basic_renderer::uniform_info>("basic-renderer-uniform"));
	basic_shader_tag.inner = temp_basic_shader;

	auto temp_triplanar_shader (pool.register_component<noob::triplanar_gradient_map_renderer::uniform_info>("triplanar-renderer-uniform"));
	triplanar_shader_tag.inner = temp_triplanar_shader;	


	// TODO: Add stage default components
	// noob::basic_renderer::uniform_info basic_shader_info;

	// basic_shader_info.colour = noob::vec4(1.0, 0.0, 0.0, 1.0);

	// auto s = shaders.add(basic_shader_info);
	// shaders.set(s, "debug");

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


noob::bodies::handle noob::stage::make_body(const noob::shapes::handle& h, float mass, const noob::vec3& pos, const noob::versor& orient)
{
	noob::body b;
	b.init(dynamics_world, shapes_holder.get(h), mass, pos, orient);
	return bodies_holder.add(b);
	//return bodies_holder.add(shapes_holder.get(h), mass, pos, orient);
}


noob::shapes::handle noob::stage::sphere(float r)
{
	auto search = spheres.find(r);
	if (search == spheres.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->sphere(r);
		auto results = spheres.insert(std::make_pair(r, shapes_holder.add(std::move(temp))));
		return (results.first)->second;

	}
	return spheres[r];
}


noob::shapes::handle noob::stage::box(float x, float y, float z)
{
	auto search = boxes.find(std::make_tuple(x,y,z));
	if (search == boxes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->box(x, y, z);
		auto results = boxes.insert(std::make_pair(std::make_tuple(x,y,z), shapes_holder.add(std::move(temp))));
		return (results.first)->second;
	}
	return boxes[std::make_tuple(x,y,z)];
}


noob::shapes::handle noob::stage::cylinder(float r, float h)
{
	auto search = cylinders.find(std::make_tuple(r, h));
	if (search == cylinders.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cylinder(r, h);
		auto results = cylinders.insert(std::make_pair(std::make_tuple(r, h), shapes_holder.add(std::move(temp))));
		return (results.first)->second;
	}
	return cylinders[std::make_tuple(r, h)];
}


noob::shapes::handle noob::stage::cone(float r, float h)
{
	auto search = cones.find(std::make_tuple(r, h));
	if (search == cones.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->cone(r, h);
		auto results = cones.insert(std::make_pair(std::make_tuple(r, h), shapes_holder.add(std::move(temp))));
		return (results.first)->second;
	}
	return cones[std::make_tuple(r, h)];
}


noob::shapes::handle noob::stage::capsule(float r, float h)
{
	auto search = capsules.find(std::make_tuple(r, h));
	if (search == capsules.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->capsule(r, h);
		auto results = capsules.insert(std::make_pair(std::make_tuple(r, h), shapes_holder.add(std::move(temp))));
		return (results.first)->second;
	}
	return capsules[std::make_tuple(r, h)];
}


noob::shapes::handle noob::stage::plane(const noob::vec3& normal, float offset)
{
	auto search = planes.find(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset));
	if (search == planes.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->plane(normal, offset);
		auto results = planes.insert(std::make_pair(std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset), shapes_holder.add(std::move(temp))));
		return (results.first)->second;
	}
	return planes[std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset)];
}


noob::shapes::handle noob::stage::make_hull(const std::vector<noob::vec3>& points)
{
	std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
	temp->convex(points);
	return shapes_holder.add(std::move(temp));
}


noob::shapes::handle noob::stage::make_trimesh(const noob::basic_mesh& mesh)
{
	std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
	temp->trimesh(mesh);
	return shapes_holder.add(std::move(temp));
}


noob::basic_models::handle noob::stage::make_basic_model(const noob::basic_mesh& input_mesh)
{
	std::unique_ptr<noob::basic_model> temp = std::make_unique<noob::basic_model>();
	temp->init(input_mesh);
	return basic_models_holder.add(std::move(temp));
}


noob::animated_models::handle noob::stage::make_animated_model(const std::string& filename)
{
	std::unique_ptr<noob::animated_model> temp = std::make_unique<noob::animated_model>();
	temp->init(filename);
	return animated_models_holder.add(std::move(temp));
}


noob::skeletal_anims::handle noob::stage::make_skeleton(const std::string& filename)
{
	std::unique_ptr<noob::skeletal_anim> temp = std::make_unique<noob::skeletal_anim>();
	temp->init(filename);
	return skeletal_anims_holder.add(std::move(temp));
}


es::entity noob::stage::actor(const noob::bodies::handle _body, const noob::movement_controllers::handle _controller, const noob::animated_models::handle _skeleton, const std::string& friendly_name)
{

}


es::entity noob::stage::prop(const noob::bodies::handle _body, const std::string& friendly_name)
{

}


es::entity noob::stage::scenery(const noob::basic_mesh& _mesh, const noob::vec3& pos, const noob::versor& orient, const std::string& friendly_name)
{

}
