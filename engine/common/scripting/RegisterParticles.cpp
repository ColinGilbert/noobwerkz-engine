#include "RegisterParticles.hpp"
#include "Particles.hpp"

void noob::register_particles(asIScriptEngine* script_engine)
{
	int r;

	r = script_engine->RegisterObjectType("particle_system_descriptor", sizeof(noob::particle_system_descriptor), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::particle_system_descriptor>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectProperty("particle_system_descriptor", "vec3 center", asOFFSET(noob::particle_system_descriptor, center)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("particle_system_descriptor", "vec3 emit_direction", asOFFSET(noob::particle_system_descriptor, emit_direction)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("particle_system_descriptor", "vec3 emit_direction_variance", asOFFSET(noob::particle_system_descriptor, emit_direction_variance)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("particle_system_descriptor", "vec3 wind", asOFFSET(noob::particle_system_descriptor, wind)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("particle_system_descriptor", "reflectance_handle reflect", asOFFSET(noob::particle_system_descriptor, reflect)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("particle_system_descriptor", "shape_handle shape", asOFFSET(noob::particle_system_descriptor, shape)); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("particle_system_descriptor", "void set_colour(uint32, const vec4& in)", asMETHOD(noob::particle_system_descriptor, set_colour), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("particle_system_descriptor", "vec4 get_colour(uint32) const", asMETHOD(noob::particle_system_descriptor, get_colour), asCALL_THISCALL); assert( r >= 0 );	
}
