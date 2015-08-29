#include "SkeletalAnim.hpp"
#include "TransformHelper.hpp"
#include "Logger.hpp"


noob::skeletal_anim::~skeletal_anim()
{
	for (auto& it : runtime_anims)
	{
		allocator->Delete(it.second);
	}

	allocator->Deallocate(model_matrices);
}


void noob::skeletal_anim::init(const std::string& filename)
{
	ozz::io::File file(filename.c_str(), "rb");
	if (!file.opened())
	{
		logger::log(fmt::format("[AnimatedModel] - load_skeleton({0}) fail. Cannot open file.", filename));
		valid = false;
		return;
	}

	ozz::io::IArchive archive(&file);
	if (!archive.TestTag<ozz::animation::Skeleton>())
	{
		logger::log(fmt::format("[AnimatedModel] - load_skeleton({0}) fail. Archive corrupt.", filename));
		valid = false;
		return;
	}

	archive >> skeleton;
	model_matrices = allocator->AllocateRange<ozz::math::Float4x4>(skeleton.num_joints());
	
	logger::log(fmt::format("[AnimatedModel] - load_skeleton({0}) success!", filename));
	
	valid = true;
}


bool noob::skeletal_anim::load_animation(const std::string& filename, const std::string& anim_name)
{
	ozz::animation::offline::RawAnimation _animation;
	ozz::io::File file(filename.c_str(), "rb");

	if (!file.opened())
	{
		return false;
	}

	ozz::io::IArchive archive(&file);
	if (!archive.TestTag<ozz::animation::offline::RawAnimation>())
	{
		return false;
	}

	archive >> _animation;

	raw_anims.insert(std::make_pair(anim_name, _animation));

	return true;
}


void noob::skeletal_anim::optimize(float translation_tolerance, float rotation_tolerance, float scale_tolerance, const std::string& name)
{
	bool optimize = !(translation_tolerance == 0.0 && rotation_tolerance == 0.0 && scale_tolerance == 0.0);
	ozz::animation::offline::AnimationBuilder builder;
	ozz::animation::offline::AnimationOptimizer optimizer;
	optimizer.translation_tolerance = translation_tolerance;
	optimizer.rotation_tolerance = rotation_tolerance;
	optimizer.scale_tolerance = scale_tolerance;
	
	// Do all of them
	if (name == "")
	{
		for (auto& it : runtime_anims)
		{
			allocator->Delete(it.second);
		}
		runtime_anims.clear();

		for (auto& it : raw_anims)
		{
			ozz::animation::offline::RawAnimation current_raw_anim = it.second;
			if (optimize)
			{
				ozz::animation::offline::RawAnimation optimized_anim;
				optimizer(current_raw_anim, &optimized_anim);
				ozz::animation::Animation* runtime_anim = builder(optimized_anim);
				runtime_anims.insert(std::make_pair(it.first, runtime_anim));
			}
			else
			{
				ozz::animation::Animation* runtime_anim = builder(current_raw_anim);
				runtime_anims.insert(std::make_pair(it.first, runtime_anim));
			}
		}
	}
	
	// Do only the named one (if it exists)
	else
	{
		auto runtime_anim_search = runtime_anims.find(name);
		if (runtime_anim_search != runtime_anims.end())
		{
			allocator->Delete(runtime_anim_search->second);
			runtime_anims.erase(name);
		}

		auto raw_anim_search = raw_anims.find(name);
		if (raw_anim_search != raw_anims.end())
		{
			ozz::animation::offline::RawAnimation raw_anim = raw_anim_search->second;

			if (optimize)
			{
				ozz::animation::offline::RawAnimation optimized_anim;
				optimizer(raw_anim, &optimized_anim);
				ozz::animation::Animation* runtime_anim = builder(optimized_anim);
				runtime_anims.insert(std::make_pair(name, runtime_anim));
			}
			else
			{
				ozz::animation::Animation* runtime_anim = builder(raw_anim);
				runtime_anims.insert(std::make_pair(name, runtime_anim));
			}
		}
	}
}


bool noob::skeletal_anim::switch_to_anim(const std::string& name)
{
	auto search = runtime_anims.find(name);
	if (search != runtime_anims.end())
	{
		current_anim_name = name;
		current_anim = search->second;
		return true;
	}
	return false;
}
	

bool noob::skeletal_anim::anim_exists(const std::string& name) const
{
	auto search = runtime_anims.find(name);
	if (search != runtime_anims.end())
	{
		return true;
	}
	return false;
}


void noob::skeletal_anim::update(float dt)
{
	noob::skeletal_anim::sampler current_sampler = create_sampler(*current_anim);
	current_sampler.controller.time = current_time;
	current_sampler.update(dt);
	current_time += dt;
	current_sampler.get_model_mats(model_matrices);
}


void noob::skeletal_anim::reset_time(float t)
{
	current_time = t;
}


std::string noob::skeletal_anim::get_current_anim() const
{
	return current_anim_name;
}


std::vector<noob::mat4> noob::skeletal_anim::get_matrices() const
{
	std::vector<noob::mat4> mats;
	mats.reserve(skeleton.num_joints());
	
	size_t num_mats = model_matrices.Size();
	for (size_t i = 0; i < num_mats; ++i)
	{
		noob::mat4 m;
		ozz::math::Float4x4 ozz_mat = model_matrices[i];
		for (size_t c = 0; c < 4; ++c)
		{
			ozz::math::StorePtr(ozz_mat.cols[c], &m[c*4]);
		}
		mats.emplace_back(m);
	}
	return mats;
}


/*
void noob::skeletal_anim::get_matrices(std::vector<noob::mat4> mats) const
{
	mats.clear();
	mats.reserve(skeleton.num_joints());
	
	size_t num_mats = model_matrices.Size();
	for (size_t i = 0; i < num_mats; ++i)
	{
		noob::mat4 m;
		ozz::math::Float4x4 ozz_mat = model_matrices[i];
		for (size_t c = 0; c < 4; ++c)
		{
			ozz::math::StorePtr(ozz_mat.cols[c], &m[c*4]);
		}
		mats.emplace_back(m);
	}
}
*/


/*
std::array<noob::vec3, 4> noob::skeletal_anim::get_skeleton_bounds() const
{

}
*/


void noob::skeletal_anim::playback_controller::update(const ozz::animation::Animation& animation, float dt)
{
	if (!paused)
	{
		float new_time = time + dt * playback_speed;
		float loops = new_time / animation.duration();
		time = new_time - floorf(loops) * animation.duration();
	}
}


void noob::skeletal_anim::playback_controller::reset()
{
	time = 0.0;
	playback_speed = 1.0;
	paused = false;
}



void noob::skeletal_anim::sampler::update(float dt)
{
	ozz::animation::SamplingJob sampling_job;
	sampling_job.animation = animation;
	sampling_job.cache = cache;
	controller.update(*animation, dt);
	sampling_job.time = controller.time;
	sampling_job.output = locals;
	if (!sampling_job.Run())
	{
		logger::log(fmt::format("[noob::skeletal_anim::sampler.update({0}) - sampling job failed.", dt));
		return;
	}

}


ozz::Range<ozz::math::SoaTransform> noob::skeletal_anim::sampler::get_local_mats() const
{
	return locals;
}



void noob::skeletal_anim::sampler::get_model_mats(ozz::Range<ozz::math::Float4x4>& models)
{
	ozz::animation::LocalToModelJob ltm_job;
	ltm_job.skeleton = skeleton;
	ltm_job.input = locals;
	ltm_job.output = models;
	if (!ltm_job.Run())
	{
		logger::log("noob::skeletal_anim::sampler.get_model_mats() - ltm job failed.");
		return;
	}
}


noob::skeletal_anim::sampler noob::skeletal_anim::create_sampler(const ozz::animation::Animation& anim)
{
	noob::skeletal_anim::sampler sampler;
	sampler.animation = const_cast<ozz::animation::Animation*>(&anim);
	sampler.skeleton = &skeleton;
	sampler.cache = allocator->New<ozz::animation::SamplingCache>(skeleton.num_joints());
	sampler.locals = allocator->AllocateRange<ozz::math::SoaTransform>(skeleton.num_soa_joints());

	return sampler;
}


void noob::skeletal_anim::destroy_sampler(noob::skeletal_anim::sampler& sampler)
{
	allocator->Deallocate(sampler.locals);
	allocator->Delete(sampler.cache);
}


// TODO: Reintegrate the blended animations into the engine.
/*
void noob::skeletal_anim::update(float dt)
{
	   for (size_t i = 0; i < num_layers; ++i)
	   {
	   noob::skeletal_anim::sampler& sampler = samplers[i];

	   sampler.controller.update(sampler.animation, dt);

	   ozz::animation::SamplingJob sampling_job;
	   sampling_job.animation = &sampler.animation;
	   sampling_job.cache = sampler.cache;
	   sampling_job.time = sampler.controller.get_time();
	   sampling_job.output = sampler.locals;

	   if (!sampling_job.Run())
	   {
	   return;
	   }
	   }

	   ozz::animation::BlendingJob::Layer layers[num_layers];
	   for (size_t i = 0; i < num_layers; ++i)
	   {
	   layers[i].transform = samplers[i].locals;
	   layers[i].weight = samplers[i].weight;
	   }

	   ozz::animation::BlendingJob blend_job;
	   blend_job.threshold = threshold;
	   blend_job.layers.begin = layers;
	   blend_job.layers.end = layers + num_layers;
	   blend_job.bind_pose = skeleton.bind_pose();
	   blend_job.output = blended_locals;

	   if (!blend_job.Run())
	   {
	   return;
	   }

	   ozz::animation::LocalToModelJob ltm_job;
	   ltm_job.skeleton = &skeleton;
	   ltm_job.input = blended_locals;
	   ltm_job.output = model_matrices;

	   if (!ltm_job.Run())
	   {
	   return;
	   }
}
*/
