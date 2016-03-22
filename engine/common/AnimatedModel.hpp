#pragma once


#include <fstream>

// #include <bgfx/bgfx.h>

#include "format.h"
#include "Drawable.hpp"
#include "MathFuncs.hpp"
#include "BasicMesh.hpp"
#include "Graphics.hpp"

namespace noob
{
	class animated_model : public drawable
	{
		public:
			struct mesh
			{
				struct vertex
				{
					vertex() : position({0.0f, 0.0f, 0.0f}), normal({0.0f, 0.0f, 0.0f}), uv({0.0f, 0.0f} ), tangent({ 0.0f, 0.0f, 0.0f }), bitangent({0.0f, 0.0f, 0.0f}), bone_indices({ 0, 0, 0, 0 }), bone_weights({ 0.0f, 0.0f, 0.0f, 0.0f}) {}
					std::array<float, 3> position, normal;
					std::array<float, 3> uv;
					std::array<float, 3> tangent, bitangent;
					std::array<uint8_t, 4> bone_indices;
					std::array<float, 4> bone_weights;

					static void init()
					{
						ms_decl
							.begin()
							.add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
							.add(bgfx::Attrib::Normal,    3, bgfx::AttribType::Float)
							.add(bgfx::Attrib::TexCoord0, 3, bgfx::AttribType::Float)
							.add(bgfx::Attrib::Tangent,   3, bgfx::AttribType::Float)
							.add(bgfx::Attrib::Bitangent, 3, bgfx::AttribType::Float)
							.add(bgfx::Attrib::Indices,   4, bgfx::AttribType::Uint8, false, true)
							.add(bgfx::Attrib::Weight,    4, bgfx::AttribType::Float)
							.end();
					}
					static bgfx::VertexDecl ms_decl;
				};

				bgfx::VertexBufferHandle vertex_buffer;
				bgfx::IndexBufferHandle index_buffer;

				std::vector<noob::animated_model::mesh::vertex> vertices;
				std::vector<uint16_t> indices;
				std::array<float, 3> dimensions;
				std::string name;
			};

			// animated_model() : references(0) {}
			~animated_model();
			// This loads from the cereal binary files prepared by the animated_model_loader class
			void init(const std::string& filename);
			// animated_model(const noob::basic_mesh&);

			void draw(uint8_t view_id, const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags = BGFX_STATE_DEFAULT) const;

			// size_t references;

		protected:
			bool ready;
			std::vector<noob::animated_model::mesh> meshes;	

	};

}
