#pragma once

// This class is a hack to allow introspection from Bullet and will be removed if/when we make our pwon physics engine.

namespace noob
{
	class collidable
	{
		friend class stage;
		protected:
			bool is_physical;
	};
}
