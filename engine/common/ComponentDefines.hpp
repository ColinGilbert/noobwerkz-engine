#pragma once


#include "Component.hpp"
#include "MathFuncs.hpp"
#include "Body.hpp"
#include "Ghost.hpp"
#include "Shape.hpp"
#include "Light.hpp"
#include "Reflectance.hpp"
#include "BasicMesh.hpp"
#include "ActiveMesh.hpp"
#include "SkeletalAnim.hpp"
#include "BasicModel.hpp"
#include "AnimatedModel.hpp"
#include "BasicRenderer.hpp"
#include "TriplanarGradientMap.hpp"
#include "AudioSample.hpp"
#include "ShadingVariant.hpp"

namespace noob
{
	typedef noob::component<noob::body> bodies_holder;
	typedef noob::component<noob::joint> joints_holder;
	typedef noob::component<noob::ghost> ghosts_holder;
	typedef noob::component<noob::shape> shapes_holder;
	typedef noob::component<noob::light> lights_holder;
	typedef noob::component<noob::reflectance> reflectances_holder;
	typedef noob::component<noob::basic_renderer::uniform> basic_shaders_holder;
	typedef noob::component<noob::triplanar_gradient_map_renderer::uniform> triplanar_shaders_holder;
	typedef noob::component<noob::shader> shaders_holder;
	typedef noob::component_dynamic<noob::basic_model> basic_models_holder;
	typedef noob::component_dynamic<noob::animated_model> animated_models_holder;
	typedef noob::component_dynamic<noob::skeletal_anim> skeletal_anims_holder;
	typedef noob::component_dynamic<noob::audio_sample> samples_holder;

	typedef handle<noob::body> body_handle;
	typedef handle<noob::joint> joint_handle;
	typedef handle<noob::ghost> ghost_handle;	
	typedef handle<noob::shape> shape_handle;
	typedef handle<noob::light> light_handle;
	typedef handle<noob::reflectance> reflectance_handle;
	typedef handle<noob::basic_renderer::uniform> basic_shader_handle;
	typedef handle<noob::triplanar_gradient_map_renderer::uniform> triplanar_shader_handle;
	typedef handle<noob::shader> shader_handle;
	typedef handle<noob::basic_model> model_handle;
	typedef handle<noob::animated_model> animated_model_handle;
	typedef handle<noob::skeletal_anim> skeletal_anim_handle;
	typedef handle<noob::audio_sample> sample_handle;
}
