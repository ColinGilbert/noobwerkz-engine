#include "Assembly.hpp"
#include <cassert>

void noob::assembly::init(btDynamicsWorld* const world, float mass, const noob::vec3f& pos, const noob::versorf& orient, bool ccd, const std::vector<noob::prop_handle> & propz, const std::vector<noob::compound_shape::child_info> & children) noexcept(true)
{

	assert(propz.size() == children.size());

	props = propz;

	compound.init(children);

	bod.init(world, noob::body_type::DYNAMIC, compound, mass, pos, orient, ccd);
}
