#pragma once

#include "NoobDefines.hpp"

#include <noob/component/component.hpp>

#include "MathFuncs.hpp"
#include "Body.hpp"
#include "Ghost.hpp"
#include "Shape.hpp"
#include "Light.hpp"
#include "Reflectance.hpp"
#include "BasicMesh.hpp"
#include "SkeletalAnim.hpp"
#include "AudioSample.hpp"
#include "TextLabel.hpp"
#include "Timing.hpp"

namespace noob
{
	typedef noob::component<noob::body> bodies_holder;
	typedef noob::component<noob::joint> joints_holder;
	typedef noob::component<noob::ghost> ghosts_holder;
	typedef noob::component<noob::shape> shapes_holder;
	typedef noob::component<noob::light> lights_holder;
	typedef noob::component<noob::reflectance> reflectances_holder;
	typedef noob::component<noob::time> times_holder;
	typedef noob::component<noob::text_label> text_label_holder;

	// TOOD: Make basic_models_holder and animated_models_holder stack-based
	typedef noob::component_dynamic<noob::skeletal_anim> skeletal_anims_holder;
	typedef noob::component_dynamic<noob::audio_sample> samples_holder;
	typedef noob::component_dynamic<std::string> strings_holder;

	typedef handle<noob::body> body_handle;
	typedef handle<noob::joint> joint_handle;
	typedef handle<noob::ghost> ghost_handle;	
	typedef handle<noob::shape> shape_handle;
	typedef handle<noob::light> light_handle;
	typedef handle<noob::reflectance> reflectance_handle;
	typedef handle<noob::time> time_handle;
	typedef handle<noob::text_label> text_label_handle;
	// typedef handle<noob::vec3> vec3_handle;


	typedef handle<noob::skeletal_anim> skeletal_anim_handle;
	typedef handle<noob::audio_sample> sample_handle;
	typedef handle<std::string> string_handle;
	
}
