#pragma once

#include "Component.hpp"
#include "Shape.hpp"
#include "Body.hpp"
#include "BasicMesh.hpp"
#include "AnimatedModel.hpp"
#include "BasicModel.hpp"
#include "SkeletalAnim.hpp"
#include "Light.hpp"
#include "ShaderVariant.hpp"
// #include "Actor.hpp"
// #include "Prop.hpp"
// #include "Scenery.hpp"

namespace noob
{
	class body;
	typedef noob::component<std::unique_ptr<noob::basic_model>> basic_model_component;	
	typedef noob::component<std::unique_ptr<noob::animated_model>> animated_model_component;
	// typedef noob::component<std::unique_ptr<noob::basic_mesh>> mesh_component;
	typedef noob::component<std::unique_ptr<noob::skeletal_anim>> skeleton_component;
	typedef noob::component<noob::light> light_component;
	typedef noob::component<noob::reflection> reflection_component;
	typedef noob::component<noob::prepared_shaders::info> shader_component;
	typedef noob::component<std::unique_ptr<noob::shape>> shape_component;
	typedef noob::component<noob::body> body_component;
	// typedef noob::component<noob::prop> prop_component;	
	// typedef noob::component<noob::actor> actor_component;
	// typedef noob::component<noob::scenery> scenery_component;
}
