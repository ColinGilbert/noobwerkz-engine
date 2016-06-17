#include "RegisterPlane.hpp"

void noob::register_plane(asIScriptEngine* script_engine)
{
	int r;

	script_engine->RegisterObjectType("plane", sizeof(noob::plane), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::plane>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );

	RegisterVector<noob::plane>("plane", script_engine); 
	
	r = script_engine->RegisterObjectBehaviour("plane", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(as_plane_constructor_wrapper_basic), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("plane", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_plane_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("plane", "void through(const vec3& in, const vec3& in, const vec3& in)", asMETHOD(noob::plane, through), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("plane", "void normalize()", asMETHOD(noob::plane, normalize), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("plane", "float signed_distance(const vec3& in) const", asMETHOD(noob::plane, signed_distance), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("plane", "vec3 normal() const", asMETHOD(noob::plane, normal), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("plane", "float offset()", asMETHOD(noob::plane, offset), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("plane", "vec3 projection(const vec3& in) const", asMETHOD(noob::plane, projection), asCALL_THISCALL); assert(r >= 0 );
}
