/*
#include "Shape.hpp"

noob::shape::~shape()
{
	if (physics_valid)
	{
		// TODO: Remove shape from physics world
		// delete inner_shape;
	}
}

void noob::shape::sphere(float radius)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::SPHERE;
		// inner_shape = new btSphereShape(radius);
	}
	scales = noob::vec3(radius*2, radius*2, radius*2);
	// dims[0] = radius;
	// inner_shape->setMargin(0.3);
	physics_valid = true;
}


void noob::shape::box(float width, float height, float depth)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::BOX;
		// inner_shape = new btBoxShape(btVector3(width, height, depth));
	}
	scales = noob::vec3(width, height, depth);
	physics_valid = true;
}


void noob::shape::cylinder(float radius, float height)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::CYLINDER;
		// inner_shape = new btCylinderShape(btVector3(radius, height/2.0, radius));
	}
	scales = noob::vec3(radius*2, height, radius*2);
	physics_valid = true;
}


void noob::shape::capsule(float radius, float height)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::CAPSULE;
		// inner_shape = new btCapsuleShape(radius, height);
	}
	scales = noob::vec3(radius*2, height, radius*2);
	physics_valid = true;
}


void noob::shape::cone(float radius, float height)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::CONE;
		// inner_shape = new btConeShape(radius, height);
	}
	scales = noob::vec3(radius*2, height, radius*2);
	physics_valid = true;
}


void noob::shape::hull(const std::vector<noob::vec3>& points)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::CONVEX;
		scales = noob::vec3(1.0, 1.0, 1.0);
		// inner_shape = new btConvexHullShape(&points[0].v[0], points.size());
		// size_t num_p = static_cast<btConvexHullShape*>(inner_shape)->getNumPoints();
		// const btVector3* points = static_cast<btConvexHullShape*>(inner_shape)->getPoints(); 
	}
	physics_valid = true;
}


void noob::shape::trimesh(const noob::basic_mesh* mesh)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::TRIMESH;
		// btTriangleMesh* phyz_mesh = new btTriangleMesh();
		
		for (size_t i = 0; i < mesh->indices.size(); i = i + 3)
		{
			uint16_t index_1 = static_cast<uint16_t>(mesh->indices[i]);
			uint16_t index_2 = static_cast<uint16_t>(mesh->indices[i+1]);
			uint16_t index_3 = static_cast<uint16_t>(mesh->indices[i+2]);

			std::array<float, 3> v1 = mesh->vertices[index_1].v;
			std::array<float, 3> v2 = mesh->vertices[index_2].v;
			std::array<float, 3> v3 = mesh->vertices[index_3].v;

			// btVector3 bv1 = btVector3(v1[0], v1[1], v1[2]);
			// btVector3 bv2 = btVector3(v2[0], v2[1], v2[2]);
			// btVector3 bv3 = btVector3(v3[0], v3[1], v3[2]);
			
			// phyz_mesh->addTriangle(bv1, bv2, bv3);
		}        
		// inner_shape = new btBvhTriangleMeshShape(phyz_mesh, true);
		// set_margin(2.5);
		scales = noob::vec3(1.0, 1.0, 1.0);
	}
	physics_valid = true;
}


// void noob::shape::plane(const noob::vec3& normal, float offset)
// {
//	if (!physics_valid)
//	{
//		shape_type = noob::shape::type::PLANE;
//		inner_shape = new btStaticPlaneShape(btVector3(normal.v[0], normal.v[1], normal.v[2]), offset);
//	}
//	physics_valid = true;
//}


void noob::shape::set_margin(float m)
{
	// inner_shape->setMargin(m);
}


float noob::shape::get_margin() const
{
	// return inner_shape->getMargin();
}


noob::vec3 noob::shape::get_scales() const
{
	return scales;
}

noob::shape::type noob::shape::get_type() const
{
	return shape_type;
}
*/
