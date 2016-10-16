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

			struct scaled_model
			{
				scaled_model() : scales(noob::vec3(1.0, 1.0, 1.0)) {}
				noob::graphics::model_handle model_h;
				// noob::reflectance_handle reflect_h;
				noob::vec3 scales;
			};

			void init(uint32_t width, uint32_t height) noexcept(true);

			void destroy() noexcept(true);

			noob::graphics::model_handle make_model(const noob::basic_mesh&) noexcept(true);	

			void frame(uint32_t width, uint32_t height) noexcept(true);

			static graphics& get_instance() noexcept(true)
			{
				static graphics the_instance;
				ptr_to_instance = &the_instance;

				return *ptr_to_instance;
			}

		protected:

			static graphics* ptr_to_instance;

			graphics() noexcept(true) {}

			graphics(const graphics& rhs) noexcept(true)
			{
				ptr_to_instance = rhs.ptr_to_instance;
			}

			graphics& operator=(const graphics& rhs) noexcept(true)
			{
				if (this != &rhs)
				{
					ptr_to_instance = rhs.ptr_to_instance;
				}
				return *this;
			}

			~graphics() noexcept(true) {}

	};
}
