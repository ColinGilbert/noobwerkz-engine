/*
#include "Actor.hpp"
//#include "TransformHelper.hpp"

void noob::actor::set_drawable(const std::weak_ptr<noob::drawable3d>& _drawable)
{
	drawable = _drawable;
	validate();
}


void noob::actor::set_skeleton(const std::weak_ptr<noob::skeletal_anim>& _anim)
{
	anim = _anim;
	validate();
}


void noob::actor::set_shader_info(const std::weak_ptr<noob::shaders::info>& _shader_info)
{
	shader_info = _shader_info;
	validate();
}


void noob::actor::validate()
{
	if (auto a = anim.lock())
	{
		anim_raw = a.get();
	}
	else
	{
		invalidate();
		return;
	}

	if (auto d = drawable.lock())
	{
		invalidate();
		drawable_raw = d.get();
	}
	else
	{
		invalidate();
		return;
	}

	if (auto i = shader_info.lock())
	{
		shader_info_raw = i.get();
	}
	else
	{
		invalidate();
		return;
	}
	
	valid = true;
}


void noob::actor::invalidate()
{
	valid = false;
	anim_raw = nullptr;
	drawable_raw = nullptr;
	shader_info_raw = nullptr;
}


void noob::actor::update()
{
	xform = body.get_transform();
}


void noob::actor::add_to_path(const std::vector<noob::vec3>& path_segment)
{
	for (noob::vec3 p : path_segment)
	{
		path.push_back(p);
	}
}


void noob::actor::clear_path()
{
	path.clear();
}


std::vector<noob::vec3> noob::actor::get_path_vector() const
{
	std::vector<noob::vec3> p;
	p.reserve(path.size());
	for (noob::vec3 v : path)
	{
		p.emplace_back(v);
	}
	return p;

}
*/
