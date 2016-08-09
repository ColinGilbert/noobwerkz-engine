#include "RegisterGraphics.hpp"

#include "Graphics.hpp"

void noob::register_graphics(asIScriptEngine* script_engine)
{
	int r;

	noob::graphics& gfx = noob::graphics::get_instance();

	r = script_engine->RegisterObjectType("texture", sizeof(noob::graphics::texture), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::graphics::texture>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	r = script_engine->RegisterObjectProperty("texture", "uint16 handle", asOFFSET(noob::graphics::texture, handle)); assert(r >= 0);
	// static noob::graphics::texture load_texture(const std::string& friendly_name, const std::string& filename, uint32_t flags);
	
	// r = script_engine->RegisterGlobalFunction("shape_handle cone_shape(float, float)", asMETHOD(noob::globals, cone_shape), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );

	r = script_engine->RegisterGlobalFunction("texture load_texture(const string& in, const string& in, uint)", asMETHOD(noob::graphics, load_texture), asCALL_THISCALL_ASGLOBAL, &gfx); assert (r >= 0);

	// static noob::graphics::shader get_shader(const std::string&);
	// static noob::graphics::texture get_texture(const std::string&);
	r = script_engine->RegisterGlobalFunction("texture get_texture(const string& in)", asMETHOD(noob::graphics, get_texture), asCALL_THISCALL_ASGLOBAL, &gfx); assert (r >= 0);
}
