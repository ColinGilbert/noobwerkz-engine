#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Graphics.hpp"

noob::graphics* noob::graphics::ptr_to_instance;

void noob::graphics::init(uint32_t width, uint32_t height) noexcept(true)
{
}

void noob::graphics::destroy() noexcept(true)
{
}

noob::graphics::model_handle noob::graphics::make_model(const noob::basic_mesh&) noexcept(true)
{
	noob::graphics::model_handle results;
	return results;
}

void noob::graphics::frame(uint32_t width, uint32_t height) noexcept(true)
{
}
