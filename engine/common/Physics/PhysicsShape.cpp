#include "PhysicsShape.hpp"

void noob::physics_shape::set_sphere(float radius)
{
	if (!created)
	{
		btSphereShape* sphere = new btSphereShape(radius);
		shape = sphere;
		type = CYLINDER;
	}
}


void noob::physics_shape::set_box(float width, float height, float depth)
{
	if (!created)
	{
		btVector3 half_extents(width / 2, height / 2, depth / 2 );
		btBoxShape* box = new btBoxShape(half_extents);
		shape = box;
		type = BOX;
	}
}


void noob::physics_shape::set_cylinder(float radius, float height)
{
	if (!created)
	{
		btVector3 half_extents(radius / 2, height / 2, radius / 2);
		btCylinderShape* cylinder = new btCylinderShape(half_extents);
		shape = cylinder;
		type = CYLINDER;
	}
}


void noob::physics_shape::set_capsule(float radius, float height)
{
	if (!created)
	{
		btCapsuleShape* capsule = new btCapsuleShape(radius, height);
		shape = capsule;
		type = CAPSULE;
	}
}


void noob::physics_shape::set_cone(float radius, float height)
{
	if (!created)
	{
		btConeShape* cone = new btConeShape(radius, height);
		shape = cone;
		type = CONE;
	}
}


void noob::physics_shape::set_convex_hull(const std::vector<noob::vec3>& points)
{
	if (!created)
	{
		btConvexHullShape* hull = new btConvexHullShape();
		for (noob::vec3 p : points)
		{
			hull->addPoint(btVector3(p[0], p[1], p[2]));
		}

		shape = hull;
		type = CONVEX;
	}
}


void noob::physics_shape::set_compound(const std::vector<noob::physics_shape> shapes)
{
	if (!created)
	{
		btCompoundShape* compound_shape = new btCompoundShape();

		for (noob::physics_shape s : shapes)
		{
			btCollisionShape* raw_shape = s.get_shape(); 
			compound_shape->addChildShape(btTransform::getIdentity(), raw_shape);
		}

		shape = compound_shape;
		type = COMPOUND;
	}
}


void noob::physics_shape::set_multisphere(const std::vector<std::tuple<noob::vec3, float>>& pos_radii)
{
	if (!created)
	{
		std::vector<btVector3> positions;
		std::vector<float> radii;

		for (auto t : pos_radii)
		{
			btVector3 pos;
			pos[0] = std::get<0>(t)[0];
			pos[1] = std::get<0>(t)[1];
			pos[2] = std::get<0>(t)[2];
			positions.push_back(pos);
			radii.push_back(std::get<1>(t));
		}

		btMultiSphereShape* multisphere_shape = new btMultiSphereShape(&positions[0], &radii[0], pos_radii.size());
		shape = multisphere_shape;
		type = MULTISPHERE;
	}
}


void noob::physics_shape::set_static_mesh(const std::vector<noob::vec3>& vertices, const std::vector<uint16_t>& indices)
{
	if (!created)
	{
		btTriangleIndexVertexArray* mesh_interface = new btTriangleIndexVertexArray();
		btIndexedMesh part;

		part.m_vertexBase = reinterpret_cast<const unsigned char*>(&vertices[0]);
		part.m_vertexStride = sizeof(&vertices[0]) * 3;
		part.m_numVertices = vertices.size();
		part.m_triangleIndexBase = reinterpret_cast<const unsigned char*>(&indices[0]);
		part.m_triangleIndexStride = sizeof(short) * 3;
		part.m_numTriangles = indices.size() / 3;
		part.m_indexType = PHY_SHORT;

		mesh_interface->addIndexedMesh(part, PHY_SHORT);

		btBvhTriangleMeshShape* trimesh_shape = new btBvhTriangleMeshShape(mesh_interface, true); // useQuantizedAabbCompression
		shape = trimesh_shape;
		type = TRIMESH;
	}
}


void noob::physics_shape::set_static_plane(const noob::vec3& normal, float constant)
{
	if (!created)
	{
		shape = new btStaticPlaneShape(btVector3(normal.v[0], normal.v[1], normal.v[2]), constant);
		type = PLANE;
	}
}


btCollisionShape* noob::physics_shape::get_shape() const
{
	return shape;
}

noob::physics_shape::shape_type noob::physics_shape::get_type() const
{
	return type;
}
