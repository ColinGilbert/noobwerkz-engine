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
		friend class stage;
		friend class body_controller;

		public:
		enum class type { SPHERE, BOX, CAPSULE, CYLINDER, CONE, CONVEX, TRIMESH };//, PLANE };

		shape() : physics_valid(false) {}
		virtual ~shape();
		
		void set_margin(float);
		float get_margin() const;

		noob::shape::type get_type() const;

		// size_t references;

		protected:
		// Initializers. Used by noob::stage
		void sphere(float radius);
		void box(float width, float height, float depth);
		void cylinder(float radius, float height);
		void capsule(float radius, float height);
		void cone(float radius, float height);
		void convex(const noob::basic_mesh*);
		void trimesh(const noob::basic_mesh*);
		
		// In this engine, we don't really use planes. This is due to the fact that their representation doesn't jive with the other parametrics.
	//	void plane(const noob::vec3& normal, float offset);
		
		// Used to calculate scale of world transform for drawing purposes (so that only one copy of the mesh gets kept in-buffer, which is a supremely useful optimization for parametric shapes)
		noob::vec3 get_scales() const;

		noob::shape::type shape_type;
		bool physics_valid;
		noob::vec3 scales;
		btCollisionShape* inner_shape;
		noob::basic_mesh* inner_mesh;
		// noob::basic_model* inner_model;
		//noob::basic_mesh inner_mesh;
	};
}
