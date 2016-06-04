#pragma once

#include "MathFuncs.hpp"
#include "Body.hpp"
#include "Ghost.hpp"
#include "Shape.hpp"
#include "Component.hpp"
#include "Light.hpp"
#include "Reflectance.hpp"
#include "BasicMesh.hpp"
#include "ActiveMesh.hpp"
#include "SkeletalAnim.hpp"
// #include "PreparedShaders.hpp"
#include "BasicModel.hpp"
#include "AnimatedModel.hpp"
#include "BasicRenderer.hpp"
#include "TriplanarGradientMap.hpp"

namespace noob
{
	typedef noob::component<std::unique_ptr<noob::basic_mesh>> meshes_holder;
	typedef noob::component<noob::body> bodies_holder;
	typedef noob::component<noob::joint> joints_holder;
	typedef noob::component<std::unique_ptr<noob::ghost>> ghosts_holder;
	typedef noob::component<std::unique_ptr<noob::shape>> shapes_holder;
	typedef noob::component<std::unique_ptr<noob::skeletal_anim>> skeletal_anims_holder;
	typedef noob::component<noob::light> lights_holder;
	typedef noob::component<noob::reflectance> reflectances_holder;
	typedef noob::component<std::unique_ptr<noob::basic_model>> basic_models_holder;
	typedef noob::component<std::unique_ptr<noob::animated_model>> animated_models_holder;
	// typedef noob::component<noob::prepared_shaders::uniform> shaders_holder;
	typedef noob::component<noob::basic_renderer::uniform> basic_shaders_holder;
	typedef noob::component<noob::triplanar_gradient_map_renderer::uniform> triplanar_shaders_holder;

	typedef meshes_holder::handle mesh_handle;
	typedef bodies_holder::handle body_handle;
	typedef joints_holder::handle joint_handle;
	typedef ghosts_holder::handle ghost_handle;	
	typedef shapes_holder::handle shape_handle;
	typedef skeletal_anims_holder::handle skeletal_anim_handle;
	typedef lights_holder::handle light_handle;
	typedef reflectances_holder::handle reflectance_handle;
	typedef basic_models_holder::handle model_handle;
	typedef animated_models_holder::handle animated_model_handle;
	// typedef shaders_holder::handle shader_handle;

	typedef basic_shaders_holder::handle basic_shader_handle;
	typedef triplanar_shaders_holder::handle triplanar_shader_handle;
}
