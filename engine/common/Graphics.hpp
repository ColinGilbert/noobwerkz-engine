#pragma once

#include <vector>
#include <map>
#include <bgfx.h>
#include <bx/readerwriter.h>

namespace noob
{
	class graphics
	{
		public:
			enum texture_type { DIFFUSE_MAP, SPECULAR_MAP, AMBIENT_MAP, EMISSIVE_MAP, NORMAL_MAP, OPACITY_MAP, DISPLACEMENT_MAP, HEIGHT_MAP, DATA };

			typedef struct
			{
				texture_type type;
				bgfx::TextureHandle handle;


			} texture;

			static std::map<std::string, noob::graphics::texture> global_textures;
			
			static void init(uint32_t width, uint32_t height)
			{

				uint32_t reset = BGFX_RESET_VSYNC;

				bgfx::reset(width, height, reset);

				bgfx::setViewClear(0
						, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
						, 0x703070ff
						, 1.0f
						, 0
						);
			}

			static void draw(uint32_t width, uint32_t height)
			{
				// Set view 0 default viewport.
				bgfx::setViewRect(0, 0, 0, width, height);

				// This dummy draw call is here to make sure that view 0 is cleared
				// if no other draw calls are submitted to view 0.
				bgfx::submit(0);

				// Advance to next frame. Rendering thread will be kicked to
				// process submitted rendering primitives.
				bgfx::frame();
			}

			void init()
			{
				file_reader = new bx::CrtFileReader;
				file_writer = new bx::CrtFileWriter;

			}

			bx::FileReaderI* get_file_reader()
			{
				return file_reader;
			}

			bx::FileWriterI* get_file_writer()
			{
				return file_writer;
			}


		protected:
			static bx::FileReaderI* file_reader;
			static bx::FileWriterI* file_writer;
	};
}
