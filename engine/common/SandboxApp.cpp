#include "Application.hpp"
#include <fstream>

struct user_data
{
	noob::model* sphere;
};


void noob::application::init()
{
	user_data* userdata = new user_data();
	userdata->sphere = new noob::model("sphere.off","models");
	std::string fontfile = *prefix + "/font/droidsans.ttf";
	droid_font->init(fontfile);
}

void noob::application::update(double delta)
{


}

void noob::application::draw()
{
	float at[3] = { 0.0f, 0.0f, 0.0f };
	float eye[3] = { 0.0f, 0.0f, -35.0f };

	float view[16];
	bx::mtxLookAt(view, eye, at);
	float proj[16];
	bx::mtxProj(proj, 60.0f, float(width)/float(height), 0.1f, 100.0f);

	bgfx::setViewTransform(0, view, proj);
	
	bgfx::setViewRect(0, 0, 0, width, height);



	noob::mat4 identity(noob::identity_mat4());


	//sphere->draw(0, &identity.m[0], program_handle);


	droid_font->change_colour(0xFFFF00FF);
	droid_font->drawtext(std::string("Font test"), 50.0f, 50.0f, (int)width, (int)height);
	droid_font->change_colour(0xFFFFFFFF);
	droid_font->drawtext(std::string("Font test 2"), 100.0f, 100.0f, (int)width, (int)height);
}
