#include "EditorUtils.hpp"
#include "Application.hpp"

void noob::application::init()
{

	logger::log("");

	noob::editor_utils::blend_channels();

	bgfx::ProgramHandle program_handle = noob::graphics::load_program("vs_current", "fs_current");

	{
		std::stringstream ss;
		ss << "Is program valid? ";
		if (bgfx::invalidHandle != program_handle.idx)
		{
			ss << "true";
		}
		else
		{
			ss << "false";
		}

		logger::log(ss.str());
	}

	noob::graphics::add_sampler("u_texture");

	noob::graphics::shader shad;
	shad.program = program_handle;
	shad.samplers.push_back(noob::graphics::get_sampler("u_texture"));
	noob::graphics::add_shader("current", shad);

	sphere = std::unique_ptr<noob::drawable>(new noob::drawable());

	std::string meshFile = *prefix + "/models/sphere.off";

	sphere->set_mesh_file(meshFile);
	sphere->load_mesh();

	noob::graphics::load_texture("grad_map", "gradient_map.tga");

	std::string fontfile = *prefix + "/font/droidsans.ttf";
	droid_font->init(fontfile);

	nvg = nvgCreate(1, 0);
	// bgfx::setViewSeq(0, true);


	logger::log("Done init");
}

void noob::application::update(double delta)
{
}

void drawEyes(struct NVGcontext* vg, float x, float y, float w, float h, float mx, float my, float t)
{
	struct NVGpaint gloss, bg;
	float ex = w *0.23f;
	float ey = h * 0.5f;
	float lx = x + ex;
	float ly = y + ey;
	float rx = x + w - ex;
	float ry = y + ey;
	float dx,dy,d;
	float br = (ex < ey ? ex : ey) * 0.5f;
	float blink = 1 - powf(sinf(t*0.5f),200)*0.8f;
	bg = nvgLinearGradient(vg, x,y+h*0.5f,x+w*0.1f,y+h, nvgRGBA(0,0,0,32), nvgRGBA(0,0,0,16));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx+3.0f,ly+16.0f, ex,ey);
	nvgEllipse(vg, rx+3.0f,ry+16.0f, ex,ey);
	nvgFillPaint(vg, bg);
	nvgFill(vg);
	bg = nvgLinearGradient(vg, x,y+h*0.25f,x+w*0.1f,y+h, nvgRGBA(220,220,220,255), nvgRGBA(128,128,128,255));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx,ly, ex,ey);
	nvgEllipse(vg, rx,ry, ex,ey);
	nvgFillPaint(vg, bg);
	nvgFill(vg);
	dx = (mx - rx) / (ex * 10);
	dy = (my - ry) / (ey * 10);
	d = sqrtf(dx*dx+dy*dy);
	if (d > 1.0f) {
		dx /= d; dy /= d;
	}
	dx *= ex*0.4f;
	dy *= ey*0.5f;
	nvgBeginPath(vg);
	nvgEllipse(vg, lx+dx,ly+dy+ey*0.25f*(1-blink), br,br*blink);
	nvgFillColor(vg, nvgRGBA(32,32,32,255));
	nvgFill(vg);
	dx = (mx - rx) / (ex * 10);
	dy = (my - ry) / (ey * 10);
	d = sqrtf(dx*dx+dy*dy);
	if (d > 1.0f) {
		dx /= d; dy /= d;
	}
	dx *= ex*0.4f;
	dy *= ey*0.5f;
	nvgBeginPath(vg);
	nvgEllipse(vg, rx+dx,ry+dy+ey*0.25f*(1-blink), br,br*blink);
	nvgFillColor(vg, nvgRGBA(32,32,32,255));
	nvgFill(vg);
	gloss = nvgRadialGradient(vg, lx-ex*0.25f,ly-ey*0.5f, ex*0.1f,ex*0.75f, nvgRGBA(255,255,255,128), nvgRGBA(255,255,255,0));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx,ly, ex,ey);
	nvgFillPaint(vg, gloss);
	nvgFill(vg);
	gloss = nvgRadialGradient(vg, rx-ex*0.25f,ry-ey*0.5f, ex*0.1f,ex*0.75f, nvgRGBA(255,255,255,128), nvgRGBA(255,255,255,0));
	nvgBeginPath(vg);
	nvgEllipse(vg, rx,ry, ex,ey);
	nvgFillPaint(vg, gloss);
	nvgFill(vg);
}

void noob::application::draw()
{
	bgfx::setViewSeq(0, true);
	nvgBeginFrame(nvg, width, height, 1.0f);
	// drawEyes(struct NVGcontext* vg, float x, float y, float w, float h, float mx, float my, float t)
	drawEyes(nvg, width - 800, height - 240, 50.0, 50.0, 0.0, 0.0, 0.0);
	nvgEndFrame(nvg);
	bgfx::setViewSeq(0, false);
	noob::vec3 cam_up(0.0, 1.0, 0.0);
	noob::vec3 cam_target(0.0, 0.0, 0.0);
	noob::vec3 cam_pos(0.0, 0.0, 5.0);
	noob::mat4 view = noob::look_at(cam_pos, cam_target, cam_up);

	// Compute projection matrix
	if (height == 0) height = 1;
	noob::mat4 proj = noob::perspective(60.0f, static_cast<float>(width)/static_cast<float>(height), 0.1f, 100.0f);

	// Compute modelview matrix
	noob::mat4 model_mat(noob::identity_mat4());

	bgfx::setViewTransform(0, &view.m[0], &proj.m[0]);

	noob::graphics::sampler samp = noob::graphics::get_sampler("u_texture");
	bgfx::setTexture(0, samp.handle, noob::graphics::get_texture("grad_map"));
	noob::graphics::shader s = noob::graphics::get_shader("current");

	sphere->draw(model_mat, s.program);

	//	droid_font->change_colour(0xFFFF00FF);
	//	droid_font->drawtext(std::string("Font test"), 50.0f, 50.0f, (int)width, (int)height);
	//	droid_font->change_colour(0xFFFFFFFF);
	//	droid_font->drawtext(std::string("Font test 2"), 100.0f, 100.0f, (int)width, (int)height);
}
