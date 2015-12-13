#include "Stage.hpp"
#include "inline_variant.hpp"

bool noob::stage::init()
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));
	
	renderer.init();

	prop_counter = scenery_counter = 0;

	// TODO: Add stage default components
	logger::log("Creating sphere");	
	auto temp_shape = sphere(1.0);
	logger::log("Creating body");
	auto temp_body = body(temp_shape, 1.0, noob::vec3(0.0, 0.0, 0.0), noob::versor(0.0, 0.0, 0.0, 1.0));
	logger::log("Creating model");
	auto temp_model = model(temp_shape);
	logger::log("Creating shader info");
	noob::basic_renderer::uniform_info basic_shader_info;
	basic_shader_info.colour = noob::vec4(1.0, 0.0, 0.0, 1.0);
	auto s = shaders.add(basic_shader_info);
	shaders.set_name(s, "debug");
	logger::log("Creating prop");
	auto temp_prop = prop(temp_body, temp_model, s);
	logger::log("Creating boxmesh");
	noob::basic_mesh temp_mesh = noob::mesh_utils::box(1000.0, 1.0, 1000.0);
	logger::log("Creating scenery");
	auto temp_scenery = scenery(temp_mesh, noob::vec3(0.0, -1.0, 0.0), noob::versor(0.0, 0.0, 0.0, 1.0), s);
	fmt::MemoryWriter ww;
	ww << "Scenery created. ID = " << temp_scenery.inner;
	logger::log(ww.str());
	// auto temp_light = light(temp_lighting_info);
	// auto temp_reflection = reflection(temp_reflect_info);

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
	for (auto p : props.items)
	{
		renderer.draw(p.get_model(), *(p.get_shading()), p.get_body()->get_transform());
	}

}


noob::model_component::handle noob::stage::model(const noob::basic_mesh& m)
{
	noob::model temp(m);
	return models.add(std::make_unique<noob::model>(temp));
}


noob::model_component::handle noob::stage::model(const noob::shape_component::handle h)
{
	noob::model m(shapes.get(h)->get_mesh());
	return models.add(std::make_unique<noob::model>(m));
}


noob::basic_mesh noob::stage::make_mesh(const noob::shape_component::handle h)
{

}


noob::model_component::handle noob::stage::model(const std::string& filename)
{
	noob::model temp(filename);
	return models.add(std::make_unique<noob::model>(temp));
}


noob::skeleton_component::handle noob::stage::skeleton(const std::string& filename)
{
	std::unique_ptr<noob::skeletal_anim> temp = std::make_unique<noob::skeletal_anim>();
	temp->init(filename);
	return skeletons.add(std::move(temp));
}

/*
noob::actor_component::handle noob::stage::actor(const noob::body_component::handle body_handle, const noob::model_component::handle model_handle, const skeleton_component::handle skeleton_handle)
{
	noob::actor temp_actor;
	temp_actor.init(bodies.get(body_handle), models.get(model_handle));
}
*/


noob::prop_component::handle noob::stage::prop(const noob::body_component::handle body_handle, const noob::model_component::handle model_handle, const noob::shader_component::handle shader_handle)
{
	// Gives a name + number in order to look up.
	std::string temp_name = "noob-prop";
	
	temp_name.append(std::to_string(prop_counter));

	++prop_counter;

	noob::prop temp_prop;
	
	temp_prop.init(bodies.get(body_handle), models.get(model_handle), shaders.get(shader_handle));
	
	return props.add(temp_prop);
}


noob::prop_component::handle noob::stage::scenery(const noob::basic_mesh& mesh, const noob::vec3& pos, const noob::versor& orient, const noob::shader_component::handle shader_handle)
{
	std::string temp_name = "noob-scenery";

	temp_name.append(std::to_string(scenery_counter));

	++scenery_counter;

	noob::shape_component::handle shape_handle = trimesh(mesh, temp_name);
	
	noob::body_component::handle body_handle = body(shape_handle, 1.0, pos, orient);
	
	noob::model_component::handle model_handle = model(mesh);
	
	// Creates prop from method properly defined in scene :)
	return prop(body_handle, model_handle, shader_handle);
}


noob::body_component::handle noob::stage::body(const shape_component::handle shape_handle, float mass, const noob::vec3& pos, const noob::versor& orient)
{
	noob::body temp_bod;
	temp_bod.init(dynamics_world, shapes.get(shape_handle), mass, pos, orient);
	return bodies.add(temp_bod);
}


void noob::stage::change_shading(noob::prop_component::handle prop_handle, noob::shader_component::handle shader_handle)
{
	noob::prop* p = props.get(prop_handle);
	noob::prepared_shaders::info* s = shaders.get(shader_handle);
	p->set_shading(s);
}



noob::light_component::handle noob::stage::light(const noob::light& arg)
{
	return lights.add(arg);
}


noob::reflection_component::handle noob::stage::reflection(const noob::reflection& arg)
{
	return reflections.add(arg);
}


noob::shader_component::handle noob::stage::shader(const noob::prepared_shaders::info& arg, const std::string& name)
{
	noob::shader_component::handle h;
	if (shaders.name_exists(name))
	{
		h = shaders.add(arg);
		shaders.set_name(h, name);
	}
	return h;
}


noob::shape_component::handle noob::stage::sphere(float r)
{
	auto search = spheres.find(r);
	if (search == spheres.end())
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->sphere(r/2.0);
		spheres[r] = shapes.add(std::move(temp));
		return spheres[r];
	}
	return spheres[r];
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
	return boxes[std::make_tuple(x,y,z)];
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
	return cylinders[std::make_tuple(r, h)];
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
	return cones[std::make_tuple(r, h)];
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
	return capsules[std::make_tuple(r, h)];
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
	return planes[std::make_tuple(normal.v[0], normal.v[1], normal.v[2], offset)];
}


noob::shape_component::handle noob::stage::hull(const std::vector<noob::vec3>& points, const std::string& name)
{
	noob::shape_component::handle h;
	if (shapes.name_exists(name))
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->convex(points);
		h = shapes.add(std::move(temp));
		shapes.set_name(h,name);
	}
	return h;
}


noob::shape_component::handle noob::stage::trimesh(const noob::basic_mesh& mesh, const std::string& name)
{
	noob::shape_component::handle h;
	if (shapes.name_exists(name))
	{
		std::unique_ptr<noob::shape> temp = std::make_unique<noob::shape>();
		temp->trimesh(mesh);
		h = shapes.add(std::move(temp));
		shapes.set_name(h, name);
	}
	return h;
}
