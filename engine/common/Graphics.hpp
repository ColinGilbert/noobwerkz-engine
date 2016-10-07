#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include <noob/component/component.hpp>

#include "NoobDefines.hpp"
#include "NoobUtils.hpp"
#include "MathFuncs.hpp"


namespace noob
{
	class graphics
	{
		static graphics* ptr_to_instance;

		graphics() noexcept(true) : instancing(false) {}

		graphics(const graphics& rhs) noexcept(true)
		{
			ptr_to_instance = rhs.ptr_to_instance;
		}

		graphics& operator=(const graphics& rhs) noexcept(true)
		{
			if (this != &rhs)
			{
				ptr_to_instance = rhs.ptr_to_instance;
			}
			return *this;
		}

		~graphics() noexcept(true) {}

		public:

		static graphics& get_instance() noexcept(true)
		{
			static graphics the_instance;
			ptr_to_instance = &the_instance;

			return *ptr_to_instance;
		}

		typedef uint16_t handle_size;
		typedef noob::handle<handle_size> shader_handle;
		typedef noob::handle<handle_size> program_handle;
		typedef noob::handle<handle_size> index_buffer_handle;
		typedef noob::handle<handle_size> texture_handle;
		typedef noob::handle<handle_size> uniform_handle;

		// BGFX_HANDLE(DynamicIndexBuffer_Handle;
		// BGFX_HANDLE(DynamicVertexBufferHandle);
		// BGFX_HANDLE(FrameBufferHandle);
		// BGFX_HANDLE(IndirectBufferHandle);
		// BGFX_HANDLE(OcclusionQueryHandle);
		// BGFX_HANDLE(VertexBufferHandle);
		// BGFX_HANDLE(VertexDeclHandle);


		enum class texture_type
		{
			BC1,          //!< DXT1
			BC2,          //!< DXT3
			BC3,          //!< DXT5
			BC4,          //!< LATC1/ATI1
			BC5,          //!< LATC2/ATI2
			BC6H,         //!< BC6H
			BC7,          //!< BC7
			ETC1,         //!< ETC1 RGB8
			ETC2,         //!< ETC2 RGB8
			ETC2A,        //!< ETC2 RGBA8
			ETC2A1,       //!< ETC2 RGB8A1
			PTC12,        //!< PVRTC1 RGB 2BPP
			PTC14,        //!< PVRTC1 RGB 4BPP
			PTC12A,       //!< PVRTC1 RGBA 2BPP
			PTC14A,       //!< PVRTC1 RGBA 4BPP
			PTC22,        //!< PVRTC2 RGBA 2BPP
			PTC24,        //!< PVRTC2 RGBA 4BPP

			UNKNOWN_COMPRESSION,      // Compressed formats above.

			R1,
			A8,
			R8,
			R8I,
			R8U,
			R8S,
			R16,
			R16I,
			R16U,
			R16F,
			R16S,
			R32I,
			R32U,
			R32F,
			RG8,
			RG8I,
			RG8U,
			RG8S,
			RG16,
			RG16I,
			RG16U,
			RG16F,
			RG16S,
			RG32I,
			RG32U,
			RG32F,
			RGB8,
			RGB8I,
			RGB8U,
			RGB8S,
			RGB9E5F,
			BGRA8,
			RGBA8,
			RGBA8I,
			RGBA8U,
			RGBA8S,
			RGBA16,
			RGBA16I,
			RGBA16U,
			RGBA16F,
			RGBA16S,
			RGBA32I,
			RGBA32U,
			RGBA32F,
			R5G6B5,
			RGBA4,
			RGB5A1,
			RGB10A2,
			R11G11B10F,

			UNKNOWN_DEPTH, // Depth formats below.

			D16,
			D24,
			D24S8,
			D32,
			D16F,
			D24F,
			D32F,
			D0S8,

			COUNT
		};

		struct uniform
		{
			void init(const std::string& name, bgfx::UniformType::Enum _type, uint16_t _count = 1)
			{
				handle = bgfx::createUniform(name.c_str(), type, _count);
				type = _type;
				count = _count;
			}

			bgfx::UniformHandle handle;
			bgfx::UniformType::Enum type;
			uint16_t count;
		};

		struct sampler
		{
			void init(const std::string& name)
			{
				handle = bgfx::createUniform(name.c_str(), bgfx::UniformType::Int1);
			}

			bgfx::UniformHandle handle;
		};


		struct texture
		{
			bgfx::TextureHandle handle;
		};

		struct shader
		{
			bgfx::ProgramHandle program;
			std::vector<noob::graphics::uniform> uniforms;
			std::vector<noob::graphics::sampler> samplers;	
		};

		enum class attrib
		{
			POSITION = 0,
			NORMAL = 1,
			TANGENT = 2,
			BITANGENT = 3,
			COLOUR_0 = 4,
			COLOUR_1 = 5,
			BONE_INDICES = 6,
			BONE_WEIGHT = 7,
			TEXCOORD_0 = 8,
			TEXCOORD_1 = 9,
			TEXCOORD_2 = 10,
			TEXCOORD_3 = 11,
			TEXCOORD_4 = 12,
			TEXCOORD_5 = 13,
			TEXCOORD_6 = 14,
			TEXCOORD_7 = 15
		};


		enum class uniform_type
		{
			int1, vec4, mat3, mat4
		};

		void init(uint32_t width, uint32_t height);
		void destroy();
		void frame(uint32_t width, uint32_t height);

		// ---------------- Asset loaders (builds from files and returns handles) -----------------
		bgfx::ShaderHandle load_shader(const std::string& filename);
		bgfx::ProgramHandle load_program(const std::string& vs_filename, const std::string& fs_filename);

		// TODO: Implement this 
		// static bgfx::ProgramHandle compile_and_load_program(const std::string& vs_source_filename, const std::string& fs_source_filename, const std::string& varyings_filename);

		noob::graphics::texture load_texture(const std::string& friendly_name, const std::string& filename, uint32_t flags);

		// ---------------- Asset creators (make assets available from getters) ----------------
		bool add_sampler(const std::string&);
		noob::graphics::uniform add_uniform(const std::string& name, bgfx::UniformType::Enum type, uint16_t count);
		bool add_shader(const std::string&, const noob::graphics::shader&);

		// ---------------- Getters -----------------
		// noob::graphics::shader get_shader(const std::string&) const;
		noob::graphics::texture get_texture(const std::string&) const;
		noob::graphics::uniform get_uniform(const std::string&) const;
		noob::graphics::sampler get_sampler(const std::string&) const;

		// ---------------- Checkers ----------------
		bool is_valid(const noob::graphics::uniform& _uniform) const noexcept(true) { return (_uniform.handle.idx != bgfx::invalidHandle); }
		bool is_valid(const noob::graphics::sampler& _sampler) const noexcept(true) { return (_sampler.handle.idx != bgfx::invalidHandle); }

		// TODO: Implement these
		// static bool is_valid(const noob::graphics::shader&);
		// static bool is_valid(bgfx::ProgramHandle);
		// static bool is_valid(bgfx::ShaderHandle);
		// static bool is_valid(bgfx::TextureHandle);

		// ---------------- Conveniences ------------
		const bgfx::Memory* get_bgfx_mem(const std::string& payload) noexcept(true) { return bgfx::copy(&payload[0], payload.size()); }

		struct memory
		{
			uint8_t* data;
			uint32_t size;
		};

		const memory get_mem(const std::string& payload) noexcept(true)
		{
			const bgfx::Memory* mem = bgfx::copy(&payload[0], payload.size());

			noob::graphics::memory results;
			results.data = mem->data;
			results.size = mem->size;

			return results;
		}

		// Uniform getters
		noob::graphics::uniform get_invalid_uniform() const noexcept(true) { return invalid_uniform; }
		noob::graphics::uniform get_colour_0() const noexcept(true) { return colour_0; } 
		noob::graphics::uniform get_colour_1() const noexcept(true) { return colour_1; }
		noob::graphics::uniform get_colour_2() const noexcept(true) { return colour_2; }
		noob::graphics::uniform get_colour_3() const noexcept(true) { return colour_3; }
		noob::graphics::uniform get_blend_0() const noexcept(true) { return blend_0; }
		noob::graphics::uniform get_blend_1() const noexcept(true) { return blend_1; }
		noob::graphics::uniform get_model_scales() const noexcept(true) { return model_scales; }
		noob::graphics::uniform get_tex_scales() const noexcept(true) { return tex_scales; }
		noob::graphics::uniform get_normal_mat() const noexcept(true) { return normal_mat; }
		noob::graphics::uniform get_normal_mat_modelspace() const noexcept(true) { return normal_mat_modelspace; }
		noob::graphics::uniform get_eye_pos() const noexcept(true) { return eye_pos; }
		noob::graphics::uniform get_eye_pos_normalized() const noexcept(true) { return eye_pos_normalized; }
		noob::graphics::uniform get_ambient() const noexcept(true) { return  ambient; }
		noob::graphics::uniform get_light_pos_radius() const noexcept(true) { return light_pos_radius; }
		noob::graphics::uniform get_light_rgb_falloff() const noexcept(true) { return light_rgb_falloff; } 
		noob::graphics::uniform get_specular_shine() const noexcept(true) { return specular_shine; }
		noob::graphics::uniform get_diffuse() const noexcept(true) { return diffuse; }
		noob::graphics::uniform get_emissive() const noexcept(true) { return emissive; }
		noob::graphics::uniform get_fog() const noexcept(true) { return fog; }
		noob::graphics::uniform get_rough_albedo_fresnel() const noexcept(true)  { return rough_albedo_fresnel; }

		noob::graphics::sampler get_invalid_texture() const noexcept(true) { return invalid_texture; }
		noob::graphics::sampler get_texture_0() const noexcept(true) { return texture_0; }

		bool instancing_supported() const noexcept(true) { return instancing; }

		// bgfx::setViewTransform(0, &view_mat.m[0], &projection_mat.m[0]);
		// bgfx::setViewRect(0, 0, 0, window_width, window_height);
		// void setViewRect(uint8_t _id, uint16_t _x, uint16_t _y, BackbufferRatio::Enum _ratio);
		// // 
		void set_view_transform(uint8_t, const noob::mat4& view, const noob::mat4& proj) noexcept(true) {}
		void set_view_rect(uint8_t, uint16_t x, uint16_t y) noexcept(true) {}
		// bgfx::setViewRect(0, 0, 0, window_width, window_height);


		void set_uniform(noob::graphics::uniform, const float*);



		protected:

		bool instancing;
		// Uniforms
		noob::graphics::uniform invalid_uniform, colour_0, colour_1, colour_2, colour_3, blend_0, blend_1, model_scales, tex_scales, normal_mat, normal_mat_modelspace, eye_pos, eye_pos_normalized, ambient, light_pos_radius, light_rgb_falloff, specular_shine, diffuse, emissive, fog, rough_albedo_fresnel;
		noob::graphics::sampler invalid_texture, texture_0;

		// ---------------- Mappings --------------------
		std::unordered_map<std::string, noob::graphics::texture> global_textures;
		std::unordered_map<std::string, noob::graphics::shader> shaders;
		std::unordered_map<std::string, noob::graphics::uniform> uniforms;
		std::unordered_map<std::string, noob::graphics::sampler> samplers;

		std::vector<noob::graphics::memory> memory_references;

	};
}
