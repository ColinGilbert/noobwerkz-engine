#include "Application.hpp"
#include <cmath>
#include <random>
#include "ContactPoint.hpp"
#include "TextureLoader.hpp"

std::unique_ptr<std::string> message_profiling, message_collision;
noob::actor_handle ah;

std::string test_message;

noob::vec3f random_vec3(float x, float y, float z)
{
	return noob::vec3f(noob::random::get() * x, noob::random::get() * y, noob::random::get() * z);
}

noob::versorf random_versor()
{
	return noob::normalize(noob::versorf(noob::random::get(), noob::random::get(), noob::random::get(), noob::random::get()));
}

bool noob::application::user_init()
{
	message_profiling = std::make_unique<std::string>("");
	message_collision = std::make_unique<std::string>("");
	noob::logger::log(noob::importance::INFO, "[Application] Begin user init!");

	noob::reflectance r;
	r.set_specular(noob::vec3f(0.1, 0.1, 0.1));
	r.set_diffuse(noob::vec3f(0.1, 0.1, 0.1));
	r.set_emissive(noob::vec3f(0.0, 0.0, 0.0));
	r.set_shine(8.0);
	r.set_albedo(0.3);
	r.set_fresnel(0.2);

	noob::globals& g = noob::get_globals();

	const noob::shape_handle scenery_shp = g.sphere_shape(10.0);// g.box_shape(50.0, 20.0, 50.0);
	const noob::scenery_handle sc_h = stage.scenery(scenery_shp, noob::vec3f(0.0, 0.0, 0.0), noob::versorf(0.0, 0.0, 0.0, 1.0));//versor_from_axis_rad(0.0, 0.0, 0.0, 1.0)); // 0 rad rotation, facing up
	const noob::reflectance_handle rh = g.reflectances.add(r);

/*
	const float actor_dims = 2.0;
	// const noob::shape_handle shp = g.sphere_shape(actor_dims);
	
	const noob::shape_handle actor_shp = g.box_shape(actor_dims, actor_dims, actor_dims);

	const uint32_t actor_count = 200;

	// TODO: Fixup
	noob::actor_blueprints bp;
	bp.bounds = actor_shp;
	bp.reflect = rh;
	bp.model = g.model_from_shape(actor_shp, actor_count*8);

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
*/
	logger::log(noob::importance::INFO, "[Application] Successfully done (C++) user init.");
	return true;
}



void noob::application::user_update(double dt)
{

	noob::globals& g = noob::get_globals();
	const noob::time nowtime = noob::clock::now();
	const noob::duration time_since_update = nowtime - last_ui_update;
	const uint64_t profiling_interval = 3000;
	
	if (noob::millis(time_since_update) > profiling_interval - 1)
	{
		const noob::profiler_snap snap = g.profile_run;
		
		message_profiling = std::make_unique<std::string>(noob::concat("[User Update] Frame time: ", noob::to_string(divide_duration(snap.total_time, profiling_interval)), std::string(", draw time: "), noob::to_string(divide_duration(snap.stage_draw_duration, profiling_interval)), ", physics time: ", noob::to_string(divide_duration(snap.stage_physics_duration, profiling_interval))));
		
		noob::logger::log(noob::importance::INFO, *message_profiling);
		
		g.profile_run.total_time = g.profile_run.stage_physics_duration = g.profile_run.stage_draw_duration = g.profile_run.sound_render_duration = noob::duration(0);
		g.profile_run.num_sound_callbacks = 0;
		
		last_ui_update = nowtime;
	}
}
