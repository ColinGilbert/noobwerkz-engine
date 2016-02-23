#pragma once


#include "Drawable.hpp"
#include "BasicMesh.hpp"
// #include "IntrusiveBase.hpp"
#include <boost/intrusive_ptr.hpp>
#include "Graphics.hpp"

namespace noob
{
	class basic_model : public drawable
	{
		public:
		template <class Archive>
			void serialize( Archive & ar )
			{
				ar(vertices, indices, ready);
			}



		basic_model() : ready(false) {}
		
		virtual ~basic_model();
		
		void init(const noob::basic_mesh&);

		void draw(uint8_t view_id, const noob::mat4& model_mat, const noob::mat4& normal_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags = BGFX_STATE_DEFAULT) const;

		protected:
		struct vertex
		{
			template <class Archive>
				void serialize( Archive & ar )
				{
					ar(position, normal, ready);
				}
			vertex() : position({0.0f, 0.0f, 0.0f}), normal({0.0f, 0.0f, 0.0f}) {}
			std::array<float, 3> position, normal;

			static void init()
			{
				ms_decl
					.begin()
					.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
					.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
					.end();
			}
			static bgfx::VertexDecl ms_decl;
		};

		std::vector<noob::basic_model::vertex> vertices;
		std::vector<uint16_t> indices;
		noob::vec3 dimensions;
		bgfx::VertexBufferHandle vertex_buffer;
		bgfx::IndexBufferHandle index_buffer;
		bool ready;
	};
}
