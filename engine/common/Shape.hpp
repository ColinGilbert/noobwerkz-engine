// This represents a conceptual geometric shape.
// Tied into the physics engine.
#pragma once


#include <vector>
#include <memory>

#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <btBulletDynamicsCommon.h>
#include "BasicMesh.hpp"
#include "MeshUtils.hpp"
#include "MathFuncs.hpp"
#include "IntrusiveBase.hpp"


namespace noob
{
	class shape 
	{
		friend class body_controller;

		public:
		enum class type { SPHERE, BOX, CAPSULE, CYLINDER, CONE, CONVEX, TRIMESH, PLANE };

		shape() : references(0), physics_valid(false), mesh_initialized(false), dims({ 0.0, 0.0, 0.0} ) {}
		virtual ~shape() { delete inner_shape; }
		
		void sphere(float radius);
		void box(float width, float height, float depth);
		void cylinder(float radius, float height);
		void capsule(float radius, float height);
		void cone(float radius, float height);
		void convex(const std::vector<noob::vec3>& points);
		void trimesh(const boost::intrusive_ptr<noob::basic_mesh>&);
		void plane(const noob::vec3& normal, float offset);

		void set_margin(float);
		float get_margin() const;

		// TODO: See how the following can be implemented in the stage
		// noob::vec3 get_render_dims() const;
		// Render transform is scaled from a the 1x1 matrix for drawing purposes (only one copy of the mesh gets kept in-buffer, which is supremely useful optimization for parametric shapes)
		// noob::mat4 get_render_mat4() const;
		// Physics transform doesn't include scale. Only really useful for plane shapes.
		// noob::mat4 get_physics_mat4() const;
		size_t references;

		protected:
		
		noob::shape::type shape_type;
		bool physics_valid, mesh_initialized;
		// Used as multiplier for drawing coordinates.
		std::array<float, 3> dims;
		btCollisionShape* inner_shape;
		boost::intrusive_ptr<noob::basic_mesh> inner_mesh;
		//noob::basic_mesh inner_mesh;
	};
}
