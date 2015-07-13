// Drawable - Loads a mesh to screen.

#pragma once

#include <map>
#include <algorithm>
#include <vector>
#include <bgfx.h>

#include <boost/intrusive_ptr.hpp>

#include <assimp/scene.h>
#include <assimp/quaternion.h>
#include <assimp/anim.h>

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Logger.hpp"
#include "Mesh.hpp"
//#include  "IntrusiveReferenceCountingFunctions.hpp"
//#include "AtomicCounter.hpp"

#include "format.h"

namespace noob
{
	class drawable3d 
	{
		public:
			drawable3d() : ready(false)
			{
				noob::graphics::mesh_vertex::init();
			}

			virtual ~drawable3d();

			void init(const noob::mesh& m);
			void draw(uint8_t view_id, const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags = BGFX_STATE_DEFAULT) const;
			// Bounding boxes are expressed in model space coordinates.
			noob::mesh::bbox_info get_bbox() const { return bbox; }
			uint32_t material_index;
			
			static std::tuple<bool, const noob::drawable3d*> get(const std::string& name);
			static void add(const std::string& name, const noob::mesh& m);

			static std::map<const std::string, std::unique_ptr<noob::drawable3d>> globals;

		protected:
			void kill_videocard_buffers();

			bool ready;
			bgfx::VertexBufferHandle vertex_buffer;
			bgfx::IndexBufferHandle index_buffer;
			std::vector<uint32_t> indices;
			std::vector<noob::graphics::mesh_vertex> bgfx_vertices;
			noob::mesh::bbox_info bbox;
	};
}
