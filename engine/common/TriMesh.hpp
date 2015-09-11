#pragma once

#include "Shape.hpp"
#include "MathFuncs.hpp"
#include "BasicMesh.hpp"

namespace noob
{
	namespace physics
	{
		class static_trimesh : protected shape
		{
			public:
				template <class Archive>
					void serialize(Archive& ar)
					{
						ar(points);
					}


				template <class Archive>
					static void load_and_construct( Archive & ar, cereal::construct<noob:physics::static_trimesh> & construct )
					{
						noob::basic_mesh mesh;
						ar(points);
						construct(mesh);
					}

			static_trimesh(const noob::basic_mesh& _mesh) : mesh(_mesh)
			{
				btTriangleMesh* phyz_mesh = new btTriangleMesh();
				for (size_t i = 0; i < mesh.indices.size(); i = i + 3)
				{
					uint16_t index_1 = mesh.indices[i];
					uint16_t index_2 = mesh.indices[i+1];
					uint16_t index_3 = mesh.indices[i+2];

					std::array<float, 3> v1 = mesh.vertices[index_1].position;
					std::array<float, 3> v2 = mesh.vertices[index_2].position;
					std::array<float, 3> v3 = mesh.vertices[index_3].position;

					btVector3 bv1 = btVector3(v1[0], v1[1], v1[2]);
					btVector3 bv2 = btVector3(v2[0], v2[1], v2[2]);
					btVector3 bv3 = btVector3(v3[0], v3[1], v3[2]);

					phyz_mesh->addTriangle(bv1, bv2, bv3);
				}        

				shape = new btBvhTriangleMeshShape(phyz_mesh, true);
				shape->setMargin(0.1);
			}

			protected:
				noob::basic_mesh mesh;
		};
	}
}
