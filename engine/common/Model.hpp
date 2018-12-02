#pragma once

#include <noob/component/component.hpp>


namespace noob
{
	struct model_basic
	{
		// bool valid = false;
		uint32_t vao = 0;
		uint32_t vertices_vbo = 0;
	};

	struct model_indexed : public model_basic
	{
		uint32_t n_vertices = 0;	
		uint32_t n_indices = 0;
		uint32_t indices_vbo = 0;
	};

	struct model_buffered : public model_basic
	{
		uint32_t max = 0;
	};

	struct terrain_model : public model_buffered
	{
		static constexpr uint32_t stride = sizeof(noob::vec4f) * 3;
	};

	struct instanced_model : public model_indexed
	{
		static constexpr uint32_t colours_stride = sizeof(noob::vec4f);
		static constexpr uint32_t matrices_stride = sizeof(noob::mat4f);

		uint32_t colours_vbo = 0;
		uint32_t matrices_vbo = 0;
		uint32_t n_instances = 0;
	};

	typedef noob::handle<instanced_model> instanced_model_handle;

	// In this engine, billboards are textured 2D items that get shown in screen-space and drawn en-masse.
	struct billboard_buffer : public model_buffered
	{
		static constexpr uint32_t stride = sizeof(vec4f) * 2;
	};

	struct billboard_vertex
	{
		noob::vec2f pos;
		noob::vec2f uv;
		noob::vec4f colour;
	};

	typedef noob::handle<noob::billboard_buffer> billboard_buffer_handle;

}
