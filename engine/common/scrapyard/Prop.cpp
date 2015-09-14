#include "Prop.hpp"
#include "TransformHelper.hpp"

void noob::prop::init(btRigidBody* _body, noob::drawable* _drawable)
{
	body = _body;
	drawable = _drawable;
}

void noob::prop::set_position(const noob::vec3& _pos)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(_pos.v[0], _pos.v[1], _pos.v[2]));
	body->setWorldTransform(t);
}


void noob::prop::set_orientation(const noob::versor& _orient)
{
	btTransform t = body->getWorldTransform();
	t.setRotation(btQuaternion(_orient.q[0], _orient.q[1], _orient.q[2], _orient.q[3]));
	body->setWorldTransform(t);
}


noob::mat4 noob::prop::get_transform() const
{
	noob::transform_helper t;
	t.translate(get_position());
	t.rotate(get_orientation());
	t.scale(drawing_scale);
	return t.get_matrix();
}


noob::vec3 noob::prop::get_position() const
{
	return body->getTransform().getPosition();
}


noob::versor noob::prop::get_orientation() const
{
	return body->getTransform().getOrientation();
}


std::string noob::prop::get_debug_info() const
{
	fmt::MemoryWriter w;
	noob::vec3 pos = get_position();
	noob::vec3 linear_vel = get_linear_velocity();
	noob::vec3 angular_vel = get_angular_velocity();
	w << "[Prop] Position: (" << pos.v[0] << ", " << pos.v[1] << ", " << pos.v[2] << "). Linear velocity: (" << linear_vel.v[0] << ", " << linear_vel.v[1] << ", " << linear_vel.v[2] << "). Angular Velocity: (" << angular_vel.v[0] << ", " << angular_vel.v[1] << ", " << angular_vel.v[2] << ")";
	return w.str();
}



void noob::prop::attach_sphere(float radius, float mass, const noob::vec3& position, const noob::versor& orientation)
{
	body->addCollisionShape(rp3d::SphereShape(radius), rp3d::Transform(rp3d::Vector3(position.v[0], position.v[1], position.v[2]), rp3d::Quaternion(orientation.q[0], orientation.q[1], orientation.q[2], orientation.q[3])), mass);
}


void noob::prop::attach_box(float width, float height, float depth, float mass, const noob::vec3& position, const noob::versor& orientation)
{
body->addCollisionShape(rp3d::BoxShape(rp3d::Vector3(width/2.0, height/2.0, depth/2.0)), rp3d::Transform(rp3d::Vector3(position.v[0], position.v[1], position.v[2]), rp3d::Quaternion(orientation.q[0], orientation.q[1], orientation.q[2], orientation.q[3])), mass);
}


void noob::prop::attach_capsule(float radius, float height, float mass, const noob::vec3& position, const noob::versor& orientation)
{
body->addCollisionShape(rp3d::CapsuleShape(radius, height), rp3d::Transform(rp3d::Vector3(position.v[0], position.v[1], position.v[2]), rp3d::Quaternion(orientation.q[0], orientation.q[1], orientation.q[2], orientation.q[3])), mass);
}


void noob::prop::attach_cylinder(float radius, float height, float mass, const noob::vec3& position, const noob::versor& orientation)
{
body->addCollisionShape(rp3d::CylinderShape(radius, height), rp3d::Transform(rp3d::Vector3(position.v[0], position.v[1], position.v[2]), rp3d::Quaternion(orientation.q[0], orientation.q[1], orientation.q[2], orientation.q[3])), mass);
}


void noob::prop::attach_cone(float radius, float height, float mass, const noob::vec3& position, const noob::versor& orientation)
{
body->addCollisionShape(rp3d::ConeShape(radius, height), rp3d::Transform(rp3d::Vector3(position.v[0], position.v[1], position.v[2]), rp3d::Quaternion(orientation.q[0], orientation.q[1], orientation.q[2], orientation.q[3])), mass);
}


void noob::prop::attach_hull(const noob::basic_mesh& hull, float mass, const noob::vec3& position, const noob::versor& orientation)
{
		rp3d::ConvexMeshShape shape(&hull.vertices[0].v[0], hull.vertices.size(), 3 * sizeof(float));
		
		for (size_t i = 0; i < hull.indices.size(); i += 3)
		{
			float first = hull.indices[i];
			float second = hull.indices[i+1];
			float third = hull.indices[i+2];

			shape.addEdge(first, second);
			shape.addEdge(first, third);
			shape.addEdge(second, third);
		}
		
		shape.setIsEdgesInformationUsed(true);
		
		body->addCollisionShape(shape, rp3d::Transform(rp3d::Vector3(position.v[0], position.v[1], position.v[2]), rp3d::Quaternion(orientation.q[0], orientation.q[1], orientation.q[2], orientation.q[3])), mass);
}

