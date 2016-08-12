#include "RegisterShadingVariant.hpp"

void noob::register_shading_variant(asIScriptEngine* script_engine)
{
	int r;

	r = script_engine->RegisterObjectType("surface", sizeof(noob::surface), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::surface>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	
	r = script_engine->RegisterObjectBehaviour("surface", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_surface_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("surface", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_surface_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	
	r = script_engine->RegisterObjectProperty("surface", "uint16 strength", asOFFSET(noob::surface, )); assert(r >= 0);	
}
