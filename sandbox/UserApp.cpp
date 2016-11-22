#include "Application.hpp"
#include <cmath>
#include <random>
#include "ContactPoint.hpp"

// #include "Shiny.h"

// TODO: Insert the callback function here
std::vector<std::tuple<noob::keyboard::keys, noob::keyboard::mod_keys, std::string>> keystrokes;
std::unique_ptr<std::string> message_profiling, message_collision;
noob::actor_handle ah;

std::string test_message;

noob::vec3 random_vec3(float x, float y, float z)
{
	return noob::vec3(noob::random::get() * x, noob::random::get() * y, noob::random::get() * z);
}

noob::versor random_versor()
{
	return noob::normalize(noob::versor(noob::random::get(), noob::random::get(), noob::random::get(), noob::random::get()));
}

bool noob::application::user_init()
{
	test_message = "Howdy hey!";
	message_profiling = std::make_unique<std::string>("");
	message_collision = std::make_unique<std::string>("");
	//message_profiling_two.resize(128);
	noob::audio_sample samp;
	bool b = samp.load_file(noob::concat(*prefix, "sounds/BlanketedLama.ogg"));
	noob::globals& g = noob::globals::get_instance();
	noob::sample_handle h = g.samples.add(std::make_unique<noob::audio_sample>(samp));	

	noob::audio_sample* s = g.samples.get(h);

	noob::reflectance r;
	r.set_specular(noob::vec3(0.1, 0.1, 0.1));
	r.set_diffuse(noob::vec3(0.1, 0.1, 0.1));
	r.set_emissive(noob::vec3(0.0, 0.0, 0.0));
	r.set_shine(8.0);
	r.set_albedo(0.3);
	r.set_fresnel(0.2);

	noob::reflectance_handle rh = g.reflectances.add(r);

	g.master_mixer.play_clip(h, 1.0);

	const float actor_dims = 2.0;
	noob::shape_handle shp = g.sphere_shape(actor_dims);

	const uint32_t actor_count = 200;

	// TODO: Fixup
	noob::actor_blueprints bp;
	bp.bounds = shp;
	bp.reflect = rh;
	bp.model = g.model_from_shape(shp, actor_count*8);

	noob::actor_blueprints_handle bph = stage.add_actor_blueprints(bp);

	const float stage_dim = static_cast<float>(actor_count);

	stage.reserve_actors(bph, actor_count * 8);

	for (uint32_t i = 0; i < actor_count; ++i)
	{	
		ah = stage.actor(bph, 0, random_vec3(stage_dim, stage_dim, stage_dim), random_versor());
	}
	
	for (uint32_t i = 0; i < actor_count; ++i)
	{	
		ah = stage.actor(bph, 1, random_vec3(stage_dim, stage_dim, -stage_dim), random_versor());
	}

	for (uint32_t i = 0; i < actor_count; ++i)
	{	
		ah = stage.actor(bph, 2, random_vec3(stage_dim, -stage_dim, stage_dim), random_versor());
	}

	for (uint32_t i = 0; i < actor_count; ++i)
	{	
		ah = stage.actor(bph, 3, random_vec3(stage_dim, -stage_dim, -stage_dim), random_versor());
	}

	for (uint32_t i = 0; i < actor_count; ++i)
	{	
		ah = stage.actor(bph, 4, random_vec3(-stage_dim, stage_dim, stage_dim), random_versor());
	}
	
	for (uint32_t i = 0; i < actor_count; ++i)
	{	
		ah = stage.actor(bph, 5, random_vec3(-stage_dim, stage_dim, -stage_dim), random_versor());
	}

	for (uint32_t i = 0; i < actor_count; ++i)
	{	
		ah = stage.actor(bph, 6, random_vec3(-stage_dim, -stage_dim, stage_dim), random_versor());
	}

	for (uint32_t i = 0; i < actor_count; ++i)
	{	
		ah = stage.actor(bph, 7, random_vec3(-stage_dim, -stage_dim, -stage_dim), random_versor());
	}

	// keystrokes.push_back(std::make_tuple(noob::keyboard::keys::NUM_5, noob::keyboard::mod_keys::NONE, "switch view (currently does nothing)"));
	logger::log(noob::importance::INFO, "[Application] Successfully done (C++) user init.");
	return true;
}



void noob::application::user_update(double dt)
{

	noob::globals& g = noob::globals::get_instance();
	noob::time nowtime = noob::clock::now();
	noob::duration time_since_update = nowtime - last_ui_update;
	const uint64_t profiling_interval = 3000;
	const uint64_t collision_display_interval = 500;
	if (noob::millis(time_since_update) > profiling_interval - 1)
	{
		
		noob::profiler_snap snap = g.profile_run;
		message_profiling = std::make_unique<std::string>(noob::concat("NoobWerkz editor - Frame time: ", pretty_print_timing(divide_duration(snap.total_time, profiling_interval)), std::string(", draw time: "), pretty_print_timing(divide_duration(snap.stage_draw_duration, profiling_interval)), ", physics time ", pretty_print_timing(divide_duration(snap.stage_physics_duration, profiling_interval)))); 
		noob::logger::log(noob::importance::INFO, *message_profiling);
		g.profile_run.total_time = g.profile_run.stage_physics_duration = g.profile_run.stage_draw_duration = time_since_update = noob::duration(0);
		last_ui_update = nowtime;
	}



/*
	if (noob::millis(time_since_update) > collision_display_interval -1)
	{
		std::vector<noob::contact_point> cps = stage.get_intersecting(ah);

		std::string s = noob::concat("[UserApp] Actor (", noob::to_string(ah.index()), "), num collisions = ", noob::to_string(cps.size()));
		
		message_collision = std::make_unique<std::string>(s);
	}
*/
	// gui.text(*(g.strings.get(noob::string_handle::make(0))), static_cast<float>(window_width - 500), static_cast<float>(window_height - 500), noob::gui::font_size::HEADER);
	gui.text(*message_profiling, 5.0, 5.0);
	gui.text(test_message, 50.0, 50.0);
}
