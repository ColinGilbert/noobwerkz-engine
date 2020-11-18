


void noob::compound_shape::init() noexcept(true)
{
	if (!physics_valid)
	{
		shape_type = noob::shape::type::MULTI;
		inner = new btCompoundShape();
		physics_valid = true;
	}
}

void noob::compound_shape::add_children(const std::vector<noob::multi_shape_info> & arg, std::vector<) noexcept(true)
{
	std::vector<float> masses(arg.size());
	for (noob::multi_shape_info info : arg)
	{
		btTransform t;
		t.setIdentity();
		t.setOrigin(noob::vec3f_to_bullet(info.pos));
		t.setRotation(noob::versorf_to_bullet(info.orient));

		noob::globals & g = noob::get_globals();

		masses.push_back(info.mass);
		static_cast<btCompoundShape*>(inner)->addChildShape(t, g.shape_from_handle(info.shape).inner);
	}
}

