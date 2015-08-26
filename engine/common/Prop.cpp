#include "Prop.hpp"

void noob::prop::init(rp3d::DynamicsWorld* w, const std::shared_ptr<noob::model>& drawable, const std::shared_ptr<noob::prepared_shaders::info>& uniforms, const noob::mat4& transform, rp3d::BodyType type)
{
	this->world = w;
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&transform.m[0]));
	body = world->createRigidBody(t);
	body->setType(type);
	shader_info = uniforms;
	model = drawable;
}

void noob::prop::set_transform(const noob::mat4& transform)
{
	rp3d::Transform _transform;
	_transform.setFromOpenGL(const_cast<rp3d::decimal*>(&transform.m[0]));
	body->setTransform(_transform);	
}


noob::mat4 noob::prop::get_transform() const
{
	noob::mat4 transform;
	body->getInterpolatedTransform().getOpenGLMatrix(&transform.m[0]);//transform();
	return transform;
}


void noob::prop::print_debug_info() const
{
	rp3d::Transform t = body->getInterpolatedTransform();
	fmt::MemoryWriter w;
	rp3d::Vector3 pos = t.getPosition();
	rp3d::Vector3 linear_vel = body->getLinearVelocity();
	rp3d::Vector3 angular_vel = body->getAngularVelocity();
	w << "[Character Controller] Position = (" <<  pos.x << ", " << pos.y << ", " << pos.z << "). Linear velocity = (" << linear_vel.x << ", " << linear_vel.y << ", " << linear_vel.z << "). Angular Velocity = (" << angular_vel.x << ", " << angular_vel.y << ", " << angular_vel.z << ")";
	logger::log(w.str());
}


void noob::prop::add_sphere(float radius, float mass, const noob::mat4& local_transform, short collision_mask, short collides_with)
{
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&local_transform.m[0]));
	//rp3d::SphereShape capsule(radius);
	rp3d::ProxyShape* p = body->addCollisionShape(rp3d::SphereShape(radius), t, mass);
	p->setCollisionCategoryBits(collides_with);
	p->setCollideWithMaskBits(collision_mask);
}


void noob::prop::add_box(float width, float height, float depth, float mass, const noob::mat4& local_transform, short collision_mask, short collides_with)
{
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&local_transform.m[0]));
	rp3d::ProxyShape* p = 	body->addCollisionShape(rp3d::BoxShape(rp3d::Vector3(width, height, depth)), t, mass);
	p->setCollisionCategoryBits(collides_with);
	p->setCollideWithMaskBits(collision_mask);
}


void noob::prop::add_cylinder(float radius, float height, float mass, const noob::mat4& local_transform, short collision_mask, short collides_with)
{
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&local_transform.m[0]));
	rp3d::ProxyShape* p = body->addCollisionShape(rp3d::CylinderShape(radius, height), t, mass);
	p->setCollisionCategoryBits(collides_with);
	p->setCollideWithMaskBits(collision_mask);
}


void noob::prop::add_capsule(float radius, float height, float mass, const noob::mat4& local_transform, short collision_mask, short collides_with)
{
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&local_transform.m[0]));
	rp3d::ProxyShape* p = body->addCollisionShape(rp3d::CapsuleShape(radius, height), t, mass);
	p->setCollisionCategoryBits(collides_with);
	p->setCollideWithMaskBits(collision_mask);
}


void noob::prop::add_cone(float radius, float height, float mass, const noob::mat4& local_transform, short collision_mask, short collides_with)
{
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&local_transform.m[0]));
	rp3d::ProxyShape* p = 	body->addCollisionShape(rp3d::ConeShape(radius, height), t, mass);
	p->setCollisionCategoryBits(collides_with);
	p->setCollideWithMaskBits(collision_mask);
}


void noob::prop::add_mesh(const std::vector<noob::basic_mesh>& meshes, float mass, const noob::mat4& local_transform, short collision_mask, short collides_with)
{
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&local_transform.m[0]));

	double accum = 0;

	for (noob::basic_mesh m : meshes)
	{
		accum += m.get_volume();
	}
	
	for (noob::basic_mesh m : meshes)
	{
		double local_mass = static_cast<double>(mass) * m.get_volume() / accum;
		
		rp3d::ConvexMeshShape shape(&m.vertices[0].v[0], m.vertices.size(), 3 * sizeof(float));
		
		for (size_t i = 0; i < m.indices.size(); i += 3)
		{
			float first = m.indices[i];
			float second = m.indices[i+1];
			float third = m.indices[i+2];

			shape.addEdge(first, second);
			shape.addEdge(first, third);
			shape.addEdge(second, third);
		}
		
		shape.setIsEdgesInformationUsed(true);
		
		rp3d::ProxyShape* p = body->addCollisionShape(shape, t, static_cast<float>(local_mass));
		p->setCollisionCategoryBits(collides_with);
		p->setCollideWithMaskBits(collision_mask);
	}

}
