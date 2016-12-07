#include "Application.hpp"
#include <cmath>
#include <random>
#include "ContactPoint.hpp"
#include "TextureLoader.hpp"

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
	message_profiling = std::make_unique<std::string>("");
	message_collision = std::make_unique<std::string>("");
	noob::logger::log(noob::importance::INFO, "[Application] Begin user init!");

	noob::reflectance r;
	r.set_specular(noob::vec3(0.1, 0.1, 0.1));
	r.set_diffuse(noob::vec3(0.1, 0.1, 0.1));
	r.set_emissive(noob::vec3(0.0, 0.0, 0.0));
	r.set_shine(8.0);
	r.set_albedo(0.3);
	r.set_fresnel(0.2);


	noob::globals& g = noob::get_globals();
	noob::graphics& gfx = noob::get_graphics();

	const std::string tex_src = noob::load_file_as_string(noob::concat(*prefix, "texture/gradient_map.tga"));

	noob::texture_loader_2d tex_data;
	tex_data.from_mem(tex_src, false);
	noob::texture_2d_handle tex_h = noob::texture_2d_handle::make_invalid();
/*
	if (tex_data.valid)
	{
		noob::texture_info inf;
		inf.pixels = tex_data.pixels;
		inf.mips = false;
		tex_h = gfx.reserve_texture_2d(tex_data.dims, inf);
	}
*/
	const noob::reflectance_handle rh = g.reflectances.add(r);

	const float actor_dims = 2.0;
	const noob::shape_handle shp = g.sphere_shape(actor_dims); // g.box_shape(actor_dims, actor_dims, actor_dims);

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
