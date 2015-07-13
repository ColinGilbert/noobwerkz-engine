#include "PhysicsWorld.hpp"
#include "Logger.hpp"

noob::physics_world::~physics_world()
{
	delete dynamics_world;
	delete solver;
	delete collision_configuration;
	delete dispatcher;
	delete broadphase;
}


void noob::physics_world::init()
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamics_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -9.81, 0));
}


void noob::physics_world::step(double delta)
{
	double seconds = delta / 1000.0f;
	auto max_substeps = 2;
	double fixed_timestep = 1.0/60.0f;
	dynamics_world->stepSimulation(seconds, max_substeps, fixed_timestep);
	if (seconds > max_substeps * fixed_timestep)
	{
		logger::log(fmt::format("[GameWorld] Warning: Timestep of {0} seconds is greater than max acceptable {1}", seconds, max_substeps * fixed_timestep));
	}
}


void noob::physics_world::apply_global_force(const noob::vec3& force, const btCollisionShape* shape, const noob::vec3& origin)
{
	
}


btRigidBody* noob::physics_world::dynamic_body(noob::physics_world::body_properties props, const noob::vec4& rotation, const noob::vec3& translation, btCollisionShape* shape)
{

	btDefaultMotionState* motion_state = new btDefaultMotionState(btTransform(btQuaternion(rotation.v[0], rotation.v[1], rotation.v[2], rotation.v[3]), btVector3(translation.v[0], translation.v[1], translation.v[2])));
	btVector3 shape_inertia; 
	shape->calculateLocalInertia(props.mass, shape_inertia);

	btRigidBody::btRigidBodyConstructionInfo rigid_properties(props.mass, motion_state, shape, btVector3(0,0,0));
	btRigidBody* rigid_body = new btRigidBody(rigid_properties);
	rigid_body->setAngularFactor(btVector3(props.angular_factor[0], props.angular_factor[1], props.angular_factor[2]));
	rigid_body->setLinearFactor(btVector3(props.linear_factor[0], props.linear_factor[1], props.linear_factor[2]));
	
	dynamics_world->addRigidBody(rigid_body, props.collision_group, props.collides_with);

	return rigid_body;
}


btSphereShape* noob::physics_world::sphere(float radius)
{
	btSphereShape* sphere = new btSphereShape(radius);
	return sphere;
}


btBoxShape* noob::physics_world::box(float width, float height, float depth)
{
	btVector3 half_extents(width / 2, height / 2, depth / 2 );
	btBoxShape* box = new btBoxShape(half_extents);
	return box;
}


btCylinderShape* noob::physics_world::cylinder(float radius, float height)
{
	btVector3 half_extents(radius / 2, height / 2, radius / 2);
	btCylinderShape* cylinder = new btCylinderShape(half_extents);
	return cylinder;
}


btCapsuleShape* noob::physics_world::capsule(float radius, float height)
{
	btCapsuleShape* capsule = new btCapsuleShape(radius, height);
	return capsule;
}


btConeShape* noob::physics_world::cone(float radius, float height)
{
	btConeShape* cone = new btConeShape(radius, height);
	return cone;
}


btConvexHullShape* noob::physics_world::convex_hull(const std::vector<noob::vec3>& points)
{
	btConvexHullShape* hull = new btConvexHullShape();
	for (noob::vec3 p : points)
	{
		hull->addPoint(btVector3(p[0], p[1], p[2]));
	}
	return hull;
}


btConvexHullShape* noob::physics_world::convex_hull(const noob::mesh& mesh)
{
	return convex_hull(mesh.vertices);
}


btCompoundShape* noob::physics_world::compound_shape(const noob::mesh& mesh)
{
	std::vector<noob::mesh> convex_meshes = mesh.convex_decomposition();
	return compound_shape(convex_meshes);
}


btCompoundShape* noob::physics_world::compound_shape(const std::vector<noob::mesh>& convex_meshes)
{
	btCompoundShape* compound_shape = new btCompoundShape();
	
	for (noob::mesh m : convex_meshes)
	{
		compound_shape->addChildShape(btTransform::getIdentity(), convex_hull(m));
	}
	
	return compound_shape;
}


btCompoundShape* noob::physics_world::compound_shape(const std::vector<btCollisionShape*>& shapes)
{
	btCompoundShape* compound_shape = new btCompoundShape();

	for (btCollisionShape* shape : shapes)
	{
		compound_shape->addChildShape(btTransform::getIdentity(), shape);
	}

	return compound_shape;
}


// (From my understanding, planes in Bullet are defined with parameters: normal, x+y+z = constant
btStaticPlaneShape* noob::physics_world::plane(const noob::vec3& normal, float constant)
{
	btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(normal.v[0], normal.v[1], normal.v[2]), constant);
	return plane;
}


btBvhTriangleMeshShape* noob::physics_world::static_mesh(const noob::mesh& mesh)
{
	btTriangleIndexVertexArray* mesh_interface = new btTriangleIndexVertexArray();
	btIndexedMesh part;

	part.m_vertexBase = reinterpret_cast<const unsigned char*>(&mesh.vertices[0]);
	part.m_vertexStride = sizeof(&mesh.vertices[0]) * 3;
	part.m_numVertices = mesh.vertices.size();
	part.m_triangleIndexBase = reinterpret_cast<const unsigned char*>(&mesh.indices[0]);
	part.m_triangleIndexStride = sizeof(short) * 3;
	part.m_numTriangles = mesh.indices.size() / 3;
	part.m_indexType = PHY_SHORT;

	mesh_interface->addIndexedMesh(part, PHY_SHORT);

	btBvhTriangleMeshShape* trimesh_shape = new btBvhTriangleMeshShape(mesh_interface, true); // useQuantizedAabbCompression
	return trimesh_shape;
}
/*

bool noob::physics_world::set_shape(const std::string& name, const btCollisionShape* shape)
{
	if (std::get<0>(get_shape(name)) == false)
	{
		named_shapes.insert(std::make_pair(name, shape));
		return true;
	}
	return false;
}


bool noob::physics_world::set_body(const std::string& name, const btRigidBody* body)
{
	if (std::get<0>(get_body(name)) == false)
	{
		named_bodies.insert(std::make_pair(name, body)); 
		return true;
	}
	return false;
}


std::tuple<bool, const btCollisionShape*> noob::physics_world::get_shape(const std::string& name)
{
	auto found = named_shapes.find(name);
	if (found != named_shapes.end())
	{
		return std::make_tuple(true, found->second);
	}
	else return std::make_tuple(false, nullptr);
}


std::tuple<bool, const btRigidBody*> noob::physics_world::get_body(const std::string& name)
{
	auto found = named_bodies.find(name);
	if (found != named_bodies.end())
	{
		return std::make_tuple(true, found->second);
	}
	else return std::make_tuple(false, nullptr);
}
*/