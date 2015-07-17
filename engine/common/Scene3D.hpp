#pragma once
#include "Config.hpp"
#include "Drawable3D.hpp"
#include "PhysicsWorld.hpp"
#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "HACDRenderer.hpp"
#include "TriplanarGradientMap.hpp"
#include "TransformHelper.hpp"
#include <string>

//#include <lemon/list_graph.h>
//#include <lemon/static_graph.h>

#include <es/entity.hpp>
#include <es/component.hpp>
#include <es/storage.hpp>



namespace noob
{
	class scene3d
	{
		public:
			void init();
			void loop();
			void draw();
	
			// NOTE: It is strongly recommended to give a unique name to each item. Unnamed items are quite difficult to access, and multiple items with the same name are intended to be removed/modified all at once.
			void add(const btRigidBody*, const noob::drawable3d*, const noob::triplanar_renderer::uniform_info&, const std::string& name = "");
			void add(const btGeneric6DofSpring2Constraint* constraint, const std::string& name = "");
			void remove_actors(const std::string& name);
			void remove_constraints(const std::string& name);
			std::vector<const btRigidBody*> get_bodies(const std::string& name) const;
			std::vector<const btGeneric6DofSpring2Constraint*> get_constraints(const std::string& name) const;

			noob::physics_world physics;

		protected:
			noob::hacd_renderer hacd_render;
			noob::triplanar_renderer triplanar_render;

			noob::mat4 view_mat;
			noob::mat4 projection_mat;
			es::storage world;
	};
}

