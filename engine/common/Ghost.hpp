/*
#pragma once

#include "reactphycs3d.h"


namespace noob
{
	class ghost
	{
		public:
			void init(rp3d::DynamicsWorld*, const noob::mat4& transform = noob:identity_mat4());

			void add_sphere(float radius, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_box(float width, float height, float depth, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_cylinder(float radius, float height, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_capsule(float radius, float height,const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_cone(float radius, float height, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			// This one creates a mesh from several convex hulls (ideally created beforehand via the noob::basic_mesh::convex_decomposition() interface.) May bugger up if meshes aren't convex
			void add_mesh(const std::vector<noob::basic_mesh>& mesh, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());

			void set_transform(const noob::mat4&);
			std::vector<rp3d::ContactManifold*> get_contact_manifolds() const;


		protected:
			rp3d::CollisionBody* body;
	};

}
*/
