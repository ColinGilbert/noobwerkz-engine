// A bunch of convenience typedefs.

#pragma once

#include <noob/component/component.hpp>
#include <noob/math/math_funcs.hpp>

#include "Body.hpp"
#include "Ghost.hpp"
#include "Shape.hpp"
#include "Light.hpp"
#include "Reflectance.hpp"
#include "SkeletalAnim.hpp"
#include "AudioSample.hpp"
#include "TextLabel.hpp"
#include "Timing.hpp"

namespace noob
{
	typedef noob::component<noob::shape> shapes_holder;
	typedef noob::component<noob::point_light> point_lights_holder;
	typedef noob::component<noob::reflectance> reflectances_holder;
	typedef noob::component<noob::text_label> text_label_holder;

	// TOOD: Make basic_models_holder and animated_models_holder stack-based
	typedef noob::component_dynamic<noob::skeletal_anim> skeletal_anims_holder;
	typedef noob::component_dynamic<noob::audio_sample> samples_holder;
	typedef noob::component_dynamic<std::string> strings_holder;

	// typedef handle<noob::time_point> timepoint_handle;
	// typedef handle<noob::text_label> text_label_handle;

}
