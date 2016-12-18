#pragma once


namespace noob
{
	struct model_basic
	{
		bool valid = false;
		uint32_t n_vertices = 0;
		uint32_t vao = 0;
		uint32_t vertices_vbo = 0;
	};

	struct model_indexed : public model_basic
	{
		uint32_t n_indices = 0;
		uint32_t indices_vbo = 0;
	};

	struct model_buffered : public model_basic
	{
		uint32_t min = 0;
		uint32_t max = 0;
	};
}
