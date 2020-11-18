#include "CompoundShape.hpp"

#include "Globals.hpp"

void noob::compound_shape::init(const std::vector<noob::compound_shape::child_info> & arg) noexcept(true)
{
	if (!physics_valid)
	{
		inner = new btCompoundShape();
		physics_valid = true;
	}

	btTransform t;
	std::vector<float> masses(arg.size());
	for (noob::compound_shape::child_info info : arg)
	{
		t.setIdentity();
		t.setOrigin(noob::vec3f_to_bullet(info.pos));
		t.setRotation(noob::versorf_to_bullet(info.orient));

		noob::globals & g = noob::get_globals();
		
		masses.push_back(info.mass);
		inner->addChildShape(t, g.shape_from_handle(info.shape).inner);
	}

	inertia = btVector3(0.0, 0.0, 0.0);
	t.setIdentity();
	inner->calculatePrincipalAxisTransform(&masses[0], t, inertia);

	physics_valid = true;	
}


void noob::compound_shape::set_self_index(uint32_t i) noexcept(true)
{
	inner->setUserIndex(i);
}

uint32_t noob::compound_shape::get_self_index() const noexcept(true)
{
	return inner->getUserIndex();
}
