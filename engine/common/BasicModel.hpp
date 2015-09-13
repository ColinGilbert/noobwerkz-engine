#include "Drawable.hpp"
#include "BasicMesh.hpp"

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
						.add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
						.add(bgfx::Attrib::Normal,    3, bgfx::AttribType::Float)
						.end();
				}
				static bgfx::VertexDecl ms_decl;
			};
			basic_model(const noob::basic_mesh&);
			~basic_model();
			void draw(uint8_t view_id, const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags) const;

		protected:
			std::vector<noob::basic_model::vertex> vertices;
			std::vector<uint16_t> indices;
			std::array<float, 3> dimensions;
			bgfx::VertexBufferHandle vertex_buffer;
			bgfx::IndexBufferHandle index_buffer;
			bool ready;

	};
}
