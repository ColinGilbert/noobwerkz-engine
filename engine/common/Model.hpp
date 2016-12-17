#pragma once


namespace noob
{
	struct model_basic
	{
		bool valid;
		uint32_t n_vertices, vao, vertices_vbo;
	};

	struct model_indexed : public model_basic
	{
		uint32_t n_indices, indices_vbo;
	};
}
