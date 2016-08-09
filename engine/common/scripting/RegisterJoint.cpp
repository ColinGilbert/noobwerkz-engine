#include "RegisterJoint.hpp"

void noob::register_joint(asIScriptEngine* script_engine)
{
	// NOT IMPLEMENTED YET 
	// bool get_spring(uint8_t) noexcept(true);
	// float get_stiffness(uint8_t) noexcept(true);
	// float get_damping(uint8_t) noexcept(true);
	// noob::vec2 get_limits(uint8_t) noexcept(true);

	int r;
	r = script_engine->RegisterObjectType("joint", sizeof(noob::joint), asOBJ_VALUE); assert(r >= 0 );

	r = script_engine->RegisterObjectBehaviour("joint", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_joint_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("joint", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_joint_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = script_engine->RegisterObjectMethod("joint", "void set_frames(const mat4& in, const mat4& in)", asMETHOD(noob::joint, set_frames), asCALL_THISCALL); assert( r >= 0 );

	r = script_engine->RegisterObjectMethod("joint", "void set_spring(uint8, bool)", asMETHOD(noob::joint, set_spring), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("joint", "void set_stiffness(uint8, bool)", asMETHOD(noob::joint, set_stiffness), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("joint", "void set_damping(uint8, float)", asMETHOD(noob::joint, set_damping), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("joint", "void set_limits(uint8, const vec2& in)", asMETHOD(noob::joint, set_limits), asCALL_THISCALL); assert( r >= 0 );

	r = script_engine->RegisterObjectMethod("joint", "vec3 get_axis(uint8) const", asMETHOD(noob::joint, get_axis), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("joint", "float get_angle(uint8) const", asMETHOD(noob::joint, get_angle), asCALL_THISCALL); assert( r >= 0 );

}
