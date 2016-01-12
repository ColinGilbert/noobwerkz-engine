// TODO: Store singletons that should only be stored once each. This means:
// Shapes
// Meshes
// Stages
// Skeletal animations
// (Possibly) models
//
#pragma once

#include <memory>

#include "BasicMesh.hpp"
#include "MeshUtils.hpp"
#include "BasicModel.hpp"
#include "AnimatedModel.hpp"
#include "Shape.hpp"
#include "Body.hpp"
#include "SkeletalAnim.hpp"
#include "Light.hpp"
#include "PreparedShaders.hpp"
#include "Component.hpp"

namespace noob
{
	typedef noob::component<std::unique_ptr<noob::basic_mesh>> meshes_holder;
	typedef noob::component<std::unique_ptr<noob::basic_model>> basic_models_holder;
	typedef noob::component<std::unique_ptr<noob::animated_model>> animated_models_holder;
	typedef noob::component<std::unique_ptr<noob::shape>> shapes_holder;
	typedef noob::component<std::unique_ptr<noob::skeletal_anim>> skeletal_anims_holder;
	typedef noob::component<noob::light> lights_holder;
	typedef noob::component<noob::reflection> reflections_holder;
	typedef noob::component<noob::prepared_shaders::uniform> shaders_holder;
	
	class globals
	{
		public:



			// Provides sane defaults in order not to crash the app in case of erroneous access.
			static void init()
			{
				
			}

			static meshes_holder meshes;
			static basic_models_holder basic_models;
			static animated_models_holder animated_models;
			static shapes_holder shapes;
			static skeletal_anims_holder skeletal_anims;
			static lights_holder lights;
			static reflections_holder reflections;
			static shaders_holder shaders;

	};

}
