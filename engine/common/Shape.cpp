#include "Shape.hpp"
#include "MeshUtils.hpp"

void noob::shape::sphere(float radius) noexcept(true) 
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::SPHERE;
		inner = new btSphereShape(radius);
	}
	scales = noob::vec3(radius*2.0, radius*2.0, radius*2.0);
	physics_valid = true;
}


void noob::shape::box(float width, float height, float depth) noexcept(true) 
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::BOX;
		inner = new btBoxShape(btVector3(width * 0.5, height * 0.5, depth * 0.5));
	}
	scales = noob::vec3(width, height, depth);
	physics_valid = true;
}

void noob::shape::hull(const std::vector<noob::vec3>& points) noexcept(true) 
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::HULL;
		scales = noob::vec3(1.0, 1.0, 1.0);
		inner = new btConvexHullShape(&points[0].v[0], points.size());
	}
	physics_valid = true;
}

void noob::shape::trimesh(const noob::mesh_3d& mesh) noexcept(true) 
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::TRIMESH;
		btTriangleMesh* phyz_mesh = new btTriangleMesh();

		for (size_t i = 0; i < mesh.indices.size(); i = i + 3)
		{
			uint16_t index_1 = static_cast<uint16_t>(mesh.indices[i]);
			uint16_t index_2 = static_cast<uint16_t>(mesh.indices[i+1]);
			uint16_t index_3 = static_cast<uint16_t>(mesh.indices[i+2]);

			std::array<float, 3> v1 = mesh.vertices[index_1].v;
			std::array<float, 3> v2 = mesh.vertices[index_2].v;
			std::array<float, 3> v3 = mesh.vertices[index_3].v;

			btVector3 bv1 = btVector3(v1[0], v1[1], v1[2]);
			btVector3 bv2 = btVector3(v2[0], v2[1], v2[2]);
			btVector3 bv3 = btVector3(v3[0], v3[1], v3[2]);

			phyz_mesh->addTriangle(bv1, bv2, bv3);
		}        
		inner = new btBvhTriangleMeshShape(phyz_mesh, true);
		// set_margin(2.5);
		scales = noob::vec3(1.0, 1.0, 1.0);
	}
	physics_valid = true;
}

void noob::shape::set_margin(float m) noexcept(true) 
{
	inner->setMargin(m);
}


float noob::shape::get_margin() const noexcept(true) 
{
	return inner->getMargin();
}

noob::vec3 noob::shape::get_scales() const noexcept(true) 
{
	return scales;
}

noob::shape::type noob::shape::get_type() const noexcept(true) 
{
	return shape_type;
}


noob::mesh_3d noob::shape::get_mesh() const noexcept(true)
{
	switch (shape_type)
	{
		case (noob::shape::type::SPHERE):
			{
				return noob::mesh_utils::sphere(scales[0] * 0.5, 2);
			}
		case (noob::shape::type::BOX):
			{
				return noob::mesh_utils::box(scales[0], scales[1], scales[2]);
			}
			/*
			   case (noob::shape::type::CYLINDER):
			   {
			   return noob::mesh_utils::cylinder(scales[0] * 0.5, scales[1], 8);
			   }
			   case (noob::shape::type::CONE):
			   {
			   return noob::mesh_utils::cone(scales[0] * 0.5, scales[1], 8);
			   }
			   */
		case (noob::shape::type::HULL):
			{
				const btVector3* btpoints = static_cast<btConvexHullShape*>(inner)->getUnscaledPoints();
				uint32_t num_points = static_cast<btConvexHullShape*>(inner)->getNumPoints();
				//std::vector<std::reference_wrapper<btVector3>> cv;
				std::vector<noob::vec3> points;
				for (uint32_t i = 0; i < num_points; ++i)
				{
					points.push_back(noob::vec3_from_bullet(*(btpoints+(i*sizeof(btVector3))))); // Man that was ugly...
				}
				return noob::mesh_utils::hull(points);
			}
		case (noob::shape::type::TRIMESH):
			{
				const btBvhTriangleMeshShape* shape_ptr = static_cast<btBvhTriangleMeshShape*>(inner);
				btVector3 scaling = shape_ptr->getLocalScaling();
				const btStridingMeshInterface* striding_mesh = shape_ptr->getMeshInterface();
				PHY_ScalarType scalar_type, index_type;
				scalar_type = index_type;
				int num_verts, scalar_stride, index_stride, num_faces;
				const unsigned char* vertex_base = 0;
				const unsigned char* index_base = 0;

				striding_mesh->getLockedReadOnlyVertexIndexBase(&vertex_base, num_verts, scalar_type, scalar_stride, &index_base, index_stride, num_faces, index_type, 0);

				size_t num_indices = num_faces * 3;

				uint32_t scalar_width, index_width;

				if (scalar_type == PHY_FLOAT)
				{
					scalar_width = sizeof(float);
				}
				else
				{
					scalar_width = sizeof(double);
				}

				if (index_width == PHY_SHORT)
				{
					index_width = sizeof(uint16_t);

				}
				else
				{
					index_width = sizeof(uint32_t);

				}

				noob::mesh_3d m;

				rde::fixed_array<btVector3, 3> triangle_verts;

				for (int tri_index = 0; tri_index < num_faces; ++tri_index)
				{
					unsigned int* gfx_base = (unsigned int*)(index_base + tri_index * index_stride);

					for (int j = 2; j >= 0; --j)
					{
						int graphics_index = index_type == PHY_SHORT ? ((unsigned short*)gfx_base)[j] : gfx_base[j];

						if (scalar_type == PHY_FLOAT)
						{
							float* graphics_base = (float*)(vertex_base + graphics_index * scalar_stride);
							triangle_verts[j] = btVector3(graphics_base[0] * scaling.getX(), graphics_base[1] * scaling.getY(), graphics_base[2] * scaling.getZ());
						}
						else
						{
							double* graphics_base = (double*)(vertex_base + graphics_index * scalar_stride);
							triangle_verts[j] = btVector3(btScalar(graphics_base[0] * scaling.getX()), btScalar(graphics_base[1] * scaling.getY()), btScalar(graphics_base[2] * scaling.getZ()));
						}
					}

					m.indices.push_back(m.vertices.size());
					m.vertices.push_back(vec3_from_bullet(triangle_verts[0]));
					m.indices.push_back(m.vertices.size());
					m.vertices.push_back(vec3_from_bullet(triangle_verts[1]));
					m.indices.push_back(m.vertices.size());
					m.vertices.push_back(vec3_from_bullet(triangle_verts[2]));
				}

				striding_mesh->unLockReadOnlyVertexBase(0);

				return m;
			}
		default:
			{
				logger::log(noob::importance::ERROR, "[Shape] Trying to turn invalid shape type into mesh! If this is build has asserts disabled, we will return a unit sphere mesh instead to prevent a crash. Dear Dev: Get your act together!");
				assert(0 && "[Shape] Tried to turn invalid shape type into mesh! This shouldn't happen in release mode. Ever.");
				return noob::mesh_utils::sphere(0.5, 2);
			}
	}
}

void noob::shape::clear() noexcept(true)
{
	if (physics_valid)
	{
		delete inner;
		scales = noob::vec3(1.0, 1.0, 1.0);
		physics_valid = false;
	}
}

void noob::shape::set_self_index(uint32_t i) noexcept(true)
{
	inner->setUserIndex(i);
}

uint32_t noob::shape::get_self_index() const noexcept(true)
{
	return inner->getUserIndex();
}
