#include "Graphics.hpp"

#include <glad/glad.h>

namespace noob
{
	class graphics_gl : public graphics
	{
		public:
		void init(uint32_t width, uint32_t height) noexcept(true) final;
		void destroy() noexcept(true) final;
		void frame(uint32_t width, uint32_t height) const noexcept(true) final;

		// ---------------- Asset loaders (builds from files and returns handles) -----------------
		noob::graphics::shader_handle load_shader(const std::string& filename) noexcept(true) final;
		noob::graphics::program_handle load_program(const std::string& vs, const std::string& fs) noexcept(true) final;

		noob::graphics::texture load_texture(const std::string& friendly_name, const std::string& filename, uint32_t flags) noexcept(true) final;

		// noob::graphics::basic_model_handle load_model(const noob::basic_mesh&) noexcept(true) final;


		// ---------------- Asset creators (make assets available from getters) ----------------
		// noob::graphics::uniform add_uniform(const std::string& name, noob::graphics::uniform_type type, uint16_t count) noexcept(true) final;
		// bool add_shader(const std::string&, const noob::graphics::shader&) noexcept(true) final;

		// bool add_sampler(const std::string&) noexcept(true) final;
		// ---------------- Getters -----------------
		// noob::graphics::shader get_shader(const std::string&) const;
		// noob::graphics::texture get_texture(const std::string&) const noexcept(true) final;
		// noob::graphics::uniform get_uniform(const std::string&) const noexcept(true) final;
		// noob::graphics::sampler get_sampler(const std::string&) const noexcept(true) final;

		void set_view_transform(uint8_t, const noob::mat4& view, const noob::mat4& proj) noexcept(true) final;
		void set_view_rect(uint8_t, uint16_t x, uint16_t y) noexcept(true) final;	

		protected:

		GLuint load_shader_gl(GLenum, const std::string& shader_src) const noexcept(true);
	};
}
