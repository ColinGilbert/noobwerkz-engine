#include "Shape.hpp"

#include "MeshUtils.hpp"

noob::shape::type noob::shape::get_type() const
{
	return shape_type;
}


void noob::shape::sphere(float radius)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::SPHERE;
		inner_shape = new btSphereShape(radius);
		inner_mesh = std::make_unique<noob::basic_mesh>(noob::mesh_utils::catmull_sphere(radius));
	}
	physics_valid = true;
}


void noob::shape::box(float width, float height, float depth)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::BOX;
		inner_shape = new btBoxShape(btVector3(width, height, depth));
		inner_mesh = std::make_unique<noob::basic_mesh>(noob::mesh_utils::box(width, height, depth));
	}
	physics_valid = true;
}


void noob::shape::cylinder(float radius, float height)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::CYLINDER;
		inner_shape = new btCylinderShape(btVector3(radius, height/2.0, radius));
		inner_mesh = std::make_unique<noob::basic_mesh>(noob::mesh_utils::cylinder(radius, height));
	}
	physics_valid = true;
}


void noob::shape::capsule(float radius, float height)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::CAPSULE;
		inner_shape = new btCapsuleShape(radius, height);
		inner_mesh = std::make_unique<noob::basic_mesh>(noob::mesh_utils::capsule(radius, height));
	}
	physics_valid = true;
}


void noob::shape::cone(float radius, float height)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::CONE;	
		inner_shape = new btConeShape(radius, height);
		inner_mesh = std::make_unique<noob::basic_mesh>(noob::mesh_utils::cone(radius, height));
	}
	physics_valid = true;
}


void noob::shape::convex(const std::vector<noob::vec3>& points)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::CONVEX;
		inner_shape = new btConvexHullShape(&inner_mesh->vertices[0].v[0], inner_mesh->vertices.size());
		inner_mesh = std::make_unique<noob::basic_mesh>(noob::mesh_utils::hull(points));
	}
	physics_valid = true;
}


void noob::shape::trimesh(const noob::basic_mesh& mesh)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::TRIMESH;
		btTriangleMesh* phyz_mesh = new btTriangleMesh();
		inner_mesh = std::make_unique<noob::basic_mesh>(mesh);
	
		for (size_t i = 0; i < mesh.indices.size(); i = i + 3)
		{
			uint16_t index_1 = mesh.indices[i];
			uint16_t index_2 = mesh.indices[i+1];
			uint16_t index_3 = mesh.indices[i+2];

			std::array<float, 3> v1 = mesh.vertices[index_1].v;
			std::array<float, 3> v2 = mesh.vertices[index_2].v;
			std::array<float, 3> v3 = mesh.vertices[index_3].v;

			btVector3 bv1 = btVector3(v1[0], v1[1], v1[2]);
			btVector3 bv2 = btVector3(v2[0], v2[1], v2[2]);
			btVector3 bv3 = btVector3(v3[0], v3[1], v3[2]);
			
			phyz_mesh->addTriangle(bv1, bv2, bv3);
		}        
		inner_shape = new btBvhTriangleMeshShape(phyz_mesh, true);
		set_margin(0.1);
	}
	physics_valid = true;
}


void noob::shape::plane(const noob::vec3& normal, float offset)
{
	if (!physics_valid)
	{
		float mesh_width = 0.5;
		float mesh_extents = 30000.0;
		
		shape_type = noob::shape::type::PLANE;
		
		// n_hat represents the normalized normal vector n
		noob::vec3 n_hat = noob::normalize(normal);
		inner_shape = new btStaticPlaneShape(btVector3(n_hat.v[0], n_hat.v[1], n_hat.v[2]), offset);
		noob::basic_mesh temp_mesh = noob::mesh_utils::box(mesh_extents, mesh_width, mesh_extents);
		temp_mesh.translate(noob::vec3(0.0, mesh_width * -0.5, 0.0));
		
		// Currently, our meshes' "plane" normal is (0, 1, 0), and we must get it to whatever n_hat is and then translate it with n_hat * offset
		noob::vec3 box_normal(n_hat.v[0], (n_hat.v[1] - 1.0), n_hat.v[2]);
		
		noob::mat4 affine_mat = noob::identity_mat4();
		affine_mat = noob::rotate_x_deg(affine_mat, box_normal.v[0] * 180.0);
		affine_mat = noob::rotate_y_deg(affine_mat, box_normal.v[1] * 180.0);
		affine_mat = noob::rotate_z_deg(affine_mat, box_normal.v[2] * 180.0);
		affine_mat = noob::translate(affine_mat, noob::vec3(box_normal.v[0] * offset, box_normal.v[1] * offset, box_normal.v[2] * offset));

		temp_mesh.transform(affine_mat);
		
		inner_mesh = std::make_unique<noob::basic_mesh>(temp_mesh);
	}
	physics_valid = true;
}


void noob::shape::set_margin(float m)
{
	margin = m;
	inner_shape->setMargin(m);
}


float noob::shape::get_margin() const
{
	return inner_shape->getMargin();
}


noob::basic_mesh noob::shape::get_mesh() const
{
	noob::basic_mesh m;
	for (noob::vec3 v : inner_mesh->vertices)
	{
		m.vertices.push_back(v);
	}
	for (noob::vec3 n : inner_mesh->normals)
	{
		m.normals.push_back(n);
	}
	for (noob::vec3 u : inner_mesh->texcoords)
	{
		m.texcoords.push_back(u);
	}
	for(uint32_t i : inner_mesh->indices)
	{
		m.indices.push_back(i);
	}

	return m;
}


btCollisionShape* noob::shape::get_raw_ptr() const
{
	return inner_shape;
}
