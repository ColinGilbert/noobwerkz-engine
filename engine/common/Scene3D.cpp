#include "Scene3D.hpp"


void noob::scene3d::init()
{
	// TODO: Add node 0 (default)
	hacd_render.init();
	triplanar_render.init();
	physics.init();
	auto drawable_component(world.register_component<const noob::drawable3d*>("drawable3d"));
	auto rigid_body_component(world.register_component<const btRigidBody*>("rigid_body"));
	auto constraint_component(world.register_component<const btGeneric6DofSpring2Constraint*>("constraint"));
	auto uniforms_component(world.register_component<noob::triplanar_renderer::uniform_info>("triplanar_uniforms"));
	auto name_component(world.register_component<const std::string>("name"));
}


void noob::scene3d::loop()
{

}


void noob::scene3d::draw()
{
	auto drawable_component = world.find_component("drawable3d");
	auto rigid_body_component = world.find_component("rigid_body");
	auto uniforms_component = world.find_component("triplanar_uniforms");

	world.for_each<const noob::drawable3d*, const btRigidBody*, noob::triplanar_renderer::uniform_info>(drawable_component, rigid_body_component, uniforms_component, [this](es::storage::iterator it, const noob::drawable3d*& drawable, const btRigidBody*& body, noob::triplanar_renderer::uniform_info& uniform)
			{
			const btMotionState* motion_state = body->getMotionState();
			btTransform bt_transform;
			motion_state->getWorldTransform(bt_transform);
			noob::mat4 mat;
			this->view_mat * mat;
			bt_transform.getOpenGLMatrix(&mat.m[0]);
			triplanar_render.draw(*drawable, mat, uniform);
			return true;
			});
}


void noob::scene3d::add(const btRigidBody* body, const noob::drawable3d* drawable, const noob::triplanar_renderer::uniform_info& uniform_info, const std::string& name)
{
	auto drawable_component = world.find_component("drawable3d");
	auto rigid_body_component = world.find_component("rigid_body");
	auto uniforms_component = world.find_component("triplanar_uniforms");
	auto name_component = world.find_component("name");

	es::entity e(world.new_entity());

	world.set(e, rigid_body_component, body);
	world.set(e, drawable_component, drawable);
	world.set(e, uniforms_component, uniform_info);
	if (name != "")
	{
		world.set(e, name_component, name);
	}
}


void noob::scene3d::add(const btGeneric6DofSpring2Constraint* constraint, const std::string& name)
{
	auto constraint_component = world.find_component("constraint");
	auto name_component = world.find_component("name");

	es::entity e(world.new_entity());
	world.set(e, constraint_component, constraint);

	if (name != "")
	{
		world.set(e, name_component, name);
	}
}


void noob::scene3d::remove_actors(const std::string& name)
{
	auto name_component = world.find_component("name");
	auto rigid_body_component = world.find_component("rigid_body");
	world.for_each<const std::string, btRigidBody*>(name_component, rigid_body_component, [this, name](es::storage::iterator it, const std::string& entity_name, btRigidBody*& body)
			{
			if (entity_name == name)
			{
			physics.remove(body);
			world.delete_entity(it);

			}
			return true;
			});
}


void noob::scene3d::remove_constraints(const std::string& name)
{
	auto name_component = world.find_component("name");
	auto constraint_component = world.find_component("constraint");
	world.for_each<const std::string, btGeneric6DofSpring2Constraint*>(name_component, constraint_component, [this, name](es::storage::iterator it, const std::string& entity_name, btGeneric6DofSpring2Constraint*& constraint)
			{
			if (entity_name == name)
			{
			physics.remove(constraint);
			world.delete_entity(it);
			}
			return true;
			});

}


std::vector<const btRigidBody*> noob::scene3d::get_bodies(const std::string& name) const
{
	std::vector<const btRigidBody*> bodies;
	auto name_component = world.find_component("name");
	auto rigid_body_component = world.find_component("rigid_body");
	world.for_each<const std::string, btRigidBody*>(name_component, rigid_body_component, [this, bodies, name](es::storage::iterator it, const std::string& entity_name, btRigidBody*& body)
			{
			if (entity_name == name)
			{
			bodies.push_back(body);
			}
			return true;
			});
	return bodies;
}


std::vector<const btGeneric6DofSpring2Constraint*> noob::scene3d::get_constraints(const std::string& name) const
{
	std::vector<const btGeneric6DofSpring2Constraint*> constraints;

	auto name_component = world.find_component("name");
	auto constraint_component = world.find_component("constraint");

	world.for_each<const std::string, btGeneric6DofSpring2Constraint*>(name_component, constraint_component, [this, constraints, name](es::storage::iterator it, const std::string& entity_name, btGeneric6DofSpring2Constraint*& constraint)
			{
			if (entity_name == name)
			{
			constraints.push_back(constraint);
			}

			return true;
			});

	return constraints;
}
