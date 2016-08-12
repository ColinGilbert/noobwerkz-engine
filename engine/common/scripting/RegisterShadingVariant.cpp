#include "RegisterShadingVariant.hpp"

void noob::register_shading_variant(asIScriptEngine* script_engine)
{
	int r;

	r = script_engine->RegisterEnum("shader_type");
	r = script_engine->RegisterEnumValue("shader_type", "BASIC", 0);
	r = script_engine->RegisterEnumValue("shader_type", "TRIPLANAR", 1);
/*
	r = script_engine->RegisterObjectType("shader", sizeof(noob::shader), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::shader>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectBehaviour("shader", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_shader_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("shader", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_shader_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("shader", "shader opEquals(const shader& in) const", asMETHODPR(noob::shader, operator==, (const noob::shader&) const, bool), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("shader", "shader_type type", asOFFSET(noob::shader, type)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("shader", "uint32 handle", asOFFSET(noob::shader, handle)); assert(r >= 0);	
*/
	r = script_engine->RegisterObjectType("surface", sizeof(noob::surface), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::surface>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectBehaviour("surface", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_surface_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("surface", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_surface_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("surface", "shader shading", asOFFSET(noob::surface, shading)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("surface", "reflectance_handle reflect", asOFFSET(noob::surface, reflect)); assert(r >= 0);	
}
