#include "RegisterReflectance.hpp"

void noob::register_reflectance(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("reflectance", sizeof(noob::reflectance), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::reflectance>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0);
	r = script_engine->RegisterObjectBehaviour("reflectance", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_reflectance_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("reflectance", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_reflectance_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("reflectance", "void set_specular(const vec3& in)", asMETHOD(noob::reflectance, set_specular), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("reflectance", "void set_specular_shiny(float)", asMETHOD(noob::reflectance, set_specular_shiny), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("reflectance", "void set_diffuse(const vec3& in)", asMETHOD(noob::reflectance, set_diffuse), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("reflectance", "void set_ambient(const vec3& in)", asMETHOD(noob::reflectance, set_ambient), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("reflectance", "void set_emissive(const vec3& in)", asMETHOD(noob::reflectance, set_emissive), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("reflectance", "void set_roughness(float)", asMETHOD(noob::reflectance, set_roughness), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("reflectance", "void set_albedo(float)", asMETHOD(noob::reflectance, set_albedo), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("reflectance", "void set_fresnel(float)", asMETHOD(noob::reflectance, set_fresnel), asCALL_THISCALL); assert( r >= 0 );	
}
