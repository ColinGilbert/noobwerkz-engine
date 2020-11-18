#include "Assembly.hpp"
#include <cassert>

void noob::assembly::init(noob::physics& world, float mass, const noob::vec3f& pos, const noob::versorf& orient, bool ccd, const std::vector<noob::prop_handle> & propz, const std::vector<noob::compound_shape::child_info> & children) noexcept(true)
{

	assert(propz.size() == children.size());

	props = propz;

	compound.init(children);

	bod.init(world.get_inner(), noob::body_type::DYNAMIC, compound, mass, pos, orient, ccd);
}

noob::mat4f noob::assembly::get_child_transform(uint32_t index) const noexcept(true)
{
	return bod.get_transform() * compound.get_child_transform(index);
}
