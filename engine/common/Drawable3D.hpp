// Drawable - Loads a mesh to screen.

#pragma once

#include <map>
#include <algorithm>
#include <vector>
#include <bgfx.h>

#include <assimp/scene.h>
#include <assimp/quaternion.h>
#include <assimp/anim.h>

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Logger.hpp"
#include "Mesh.hpp"

#include "format.h"

namespace noob
{
	class drawable 
	{
		public:
			drawable();
			virtual ~drawable();

			void init(const noob::mesh& m);
			void draw(uint8_t view_id, const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags = BGFX_STATE_DEFAULT) const;
			// These are expressed in model space coorsinates.
			noob::mesh::bbox_info get_bbox() const { return bbox; }
			// noob::transform get_transform() const;
			uint32_t material_index;

		protected:
			void kill_videocard_buffers();
			bool dirty;

			bgfx::VertexBufferHandle vertex_buffer;
			bgfx::IndexBufferHandle index_buffer;

			std::vector<uint32_t> indices;
			std::vector<noob::graphics::mesh_vertex> bgfx_vertices;

			noob::mesh::bbox_info bbox;
	};
}
