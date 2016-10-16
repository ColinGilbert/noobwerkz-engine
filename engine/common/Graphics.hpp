#pragma once

#include <string>

#include <noob/component/component.hpp>

#include "MathFuncs.hpp"
#include "BasicMesh.hpp"

namespace noob
{
	class graphics
	{
		public:
			typedef noob::handle<uint32_t> model_handle;

			void init(uint32_t width, uint32_t height) noexcept(true);
			void destroy() noexcept(true);

			noob::graphics::model_handle make_model(const noob::basic_mesh&) noexcept(true);	
			void frame(uint32_t width, uint32_t height) noexcept(true);
	};
}
