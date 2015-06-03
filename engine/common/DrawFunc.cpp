#include <RuntimeObjectSystem/ObjectInterfacePerModule.h>
#include <RuntimeObjectSystem/IObject.h>

#include "UserData.hpp"
#include "DrawFunc.hpp"

class UserDrawFunc : public TInterface<IID_DRAW, DrawFunc>
{
	public:

		virtual void draw(float width, float height, double delta)
		{
			noob::vec3 cam_up(0.0, 1.0, 0.0);
			noob::vec3 cam_target(0.0, 0.0, 0.0);
			noob::vec3 cam_pos(180.0, 180.0, 400.0);
			noob::mat4 view_mat = noob::look_at(cam_pos, cam_target, cam_up);
			noob::mat4 proj = noob::perspective(60.0f, noob::data->window_width / noob::data->window_height, 0.1f, 2000.0f);
			bgfx::setViewTransform(0, &view_mat.m[0], &proj.m[0]);
			bgfx::setViewRect(0, 0, 0, noob::data->window_width, noob::data->window_height);
			noob::mat4 model_mat(noob::identity_mat4());

			noob::vec4 colour_1(0.0, 0.3, 0.3, 1.0);
			noob::vec4 colour_2(0.0, 0.5, 0.5, 1.0);
			noob::vec4 colour_3(0.3, 0.3, 0.3, 1.0);
			noob::vec4 colour_4(0.2, 0.6, 0.4, 1.0);
			noob::vec4 mapping_blend(0.0, 2.0, 1.0, 0.0);
			noob::vec4 colour_positions(0.3, 0.6, 0.0, 0.0);
			noob::vec4 scales(4, 4, 4, 0);

			bgfx::setUniform(noob::graphics::get_uniform("colour_1").handle, &colour_1.v[0]);
			bgfx::setUniform(noob::graphics::get_uniform("colour_2").handle, &colour_2.v[0]);
			bgfx::setUniform(noob::graphics::get_uniform("colour_3").handle, &colour_3.v[0]);
			bgfx::setUniform(noob::graphics::get_uniform("colour_4").handle, &colour_4.v[0]);
			bgfx::setUniform(noob::graphics::get_uniform("mapping_blend").handle, &mapping_blend.v[0]);
			bgfx::setUniform(noob::graphics::get_uniform("colour_positions").handle, &colour_positions.v[0]);
			bgfx::setUniform(noob::graphics::get_uniform("scales").handle, &scales.v[0]);

			noob::graphics::sampler samp = noob::graphics::get_sampler("u_texture");
			bgfx::setTexture(0, samp.handle, noob::graphics::get_texture("grad_map"));
			noob::graphics::shader s = noob::graphics::get_shader("current");
			noob::data->sphere->draw(0, model_mat, s.program);
			bgfx::submit(0);


			noob::data->droid_font->draw_text(1, "The goat stumbled upon the cheese", 150.0, 50.0);
		}
};

REGISTERCLASS(UserDrawFunc);
