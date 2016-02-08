#pragma once

#include "BasicMesh.hpp"
#include "MathFuncs.hpp"

namespace noob
{
	class csg
	{
		public:
			void init();
			void shutdown();

			noob::basic_mesh to_basic_mesh() const;

		protected:
	};
}
