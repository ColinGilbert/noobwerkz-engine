#include "DebugDraw.hpp"



void noob::debug_draw::drawLine(const btVector3& from, const btVector3& to, const btVector3& colour)
{
	line_info l;
	l.from = noob::vec3(from);
	l.to = noob::vec3(to);
	l.colour = noob::vec3(colour);
	debug_lines.push_back(l);
	//debug_info.push_back(std::make_tuple(noob::vec3(from), noob::vec3(to), noob::vec3(colour)));
}


void noob::debug_draw::drawContactPoint(const btVector3& point_on_b, const btVector3& normal_on_b, btScalar distance, int lifetime, const btVector3& colour)
{

}


void noob::debug_draw::reportErrorWarning(const char *warning_string)
{
	logger::log(fmt::format("[Physics] {0}", warning_string));
}


void noob::debug_draw::draw3dText(const btVector3& location, const char* text_string)
{
	text_info t;
	t.pos = noob::vec2(location[0], location[1]);
	t.text = std::string(text_string);
	debug_text.push_back(t);
}


void noob::debug_draw::setDebugMode(int debug_mode)
{

}


int noob::debug_draw::getDebugMode() const
{
	return _debug_mode;
}


void noob::debug_draw::frame()
{
	// debug_info.clear();	
}
