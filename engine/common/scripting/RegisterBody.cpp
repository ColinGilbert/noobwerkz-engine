#include "RegisterBody.hpp"

void noob::register_body(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("body", sizeof(noob::body), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::body>() | asOBJ_APP_CLASS_ALLINTS ); assert(r >= 0 );

	r = script_engine->RegisterEnum("body_type"); assert(r >= 0);
	r = script_engine->RegisterEnumValue("body_type", "DYNAMIC", 0); assert(r >= 0);
	r = script_engine->RegisterEnumValue("body_type", "STATIC", 1); assert(r >= 0);
	r = script_engine->RegisterEnumValue("body_type", "KINEMATIC", 2); assert(r >= 0);
	// r = script_engine->RegisterEnumValue("body_type", "GHOST", 3); assert(r >= 0);

	r = script_engine->RegisterObjectType("body_info", sizeof(noob::body::info), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::body::info>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "float mass", asOFFSET(noob::body::info, mass)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "float friction", asOFFSET(noob::body::info, friction)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "float restitution", asOFFSET(noob::body::info, restitution)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "vec3 linear_factor", asOFFSET(noob::body::info, linear_factor)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "vec3 angular_factor", asOFFSET(noob::body::info, angular_factor)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "vec3 position", asOFFSET(noob::body::info, position)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "vec3 linear_velocity", asOFFSET(noob::body::info, linear_velocity)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "vec3 angular_velocity", asOFFSET(noob::body::info, angular_velocity)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "versor orientation", asOFFSET(noob::body::info, orientation)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "bool ccd", asOFFSET(noob::body::info, ccd)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "body_type type", asOFFSET(noob::body::info, type)); assert( r >= 0 );
}
