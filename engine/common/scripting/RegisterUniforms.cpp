#include "RegisterUniforms.hpp"


void noob::register_uniforms(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("basic_uniform", sizeof(noob::basic_renderer::uniform), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::basic_renderer::uniform>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectProperty("basic_uniform", "vec4 colour", asOFFSET(noob::basic_renderer::uniform, colour)); assert(r >= 0);

	r = script_engine->RegisterObjectType("triplanar_gradmap_uniform", sizeof(noob::triplanar_gradient_map_renderer::uniform), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::triplanar_gradient_map_renderer::uniform>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectProperty("triplanar_gradmap_uniform", "vec4 blend", asOFFSET(noob::triplanar_gradient_map_renderer::uniform, blend)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("triplanar_gradmap_uniform", "vec4 scales", asOFFSET(noob::triplanar_gradient_map_renderer::uniform, scales)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("triplanar_gradmap_uniform", "vec4 colour_positions", asOFFSET(noob::triplanar_gradient_map_renderer::uniform, colour_positions)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("triplanar_gradmap_uniform", "texture texture_map", asOFFSET(noob::triplanar_gradient_map_renderer::uniform, texture_map)); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("triplanar_gradmap_uniform", "void set_colour(uint, const vec4& in)", asMETHOD(noob::triplanar_gradient_map_renderer::uniform, set_colour), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("triplanar_gradmap_uniform", "vec4 get_colour(uint) const", asMETHOD(noob::triplanar_gradient_map_renderer::uniform, get_colour), asCALL_THISCALL); assert( r >= 0 );	

	// r = script_engine->RegisterObjectType("lit_triplanar_gradmap_uniform", sizeof(noob::triplanar_gradient_map_renderer_lit::uniform), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::triplanar_gradient_map_renderer_lit::uniform>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("lit_triplanar_gradmap_uniform", "vec4 blend", asOFFSET(noob::triplanar_gradient_map_renderer_lit::uniform, blend)); assert(r >= 0);
	// r = script_engine->RegisterObjectProperty("lit_triplanar_gradmap_uniform", "vec4 scales", asOFFSET(noob::triplanar_gradient_map_renderer_lit::uniform, scales)); assert(r >= 0);
	// r = script_engine->RegisterObjectProperty("lit_triplanar_gradmap_uniform", "vec4 colour_positions", asOFFSET(noob::triplanar_gradient_map_renderer_lit::uniform, colour_positions)); assert(r >= 0);
	// r = script_engine->RegisterObjectProperty("lit_triplanar_gradmap_uniform", "texture texture_map", asOFFSET(noob::triplanar_gradient_map_renderer_lit::uniform, texture_map)); assert(r >= 0);
	// r = script_engine->RegisterObjectMethod("lit_triplanar_gradmap_uniform", "void set_colour(uint, const vec4& in)", asMETHOD(noob::triplanar_gradient_map_renderer_lit::uniform, set_colour), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("lit_triplanar_gradmap_uniform", "vec4 get_colour(uint) const", asMETHOD(noob::triplanar_gradient_map_renderer_lit::uniform, get_colour), asCALL_THISCALL); assert( r >= 0 );	
}
