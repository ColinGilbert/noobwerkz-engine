#include "RegisterActor.hpp"
#include "ComponentDefines.hpp"


void noob::register_actor(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("actor_blueprints", sizeof(noob::actor_blueprints), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::actor_blueprints>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);

	r = script_engine->RegisterObjectBehaviour("actor_blueprints", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_actor_blueprints_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("actor_blueprints", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_actor_blueprints_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = script_engine->RegisterObjectProperty("actor_blueprints", "uint16 strength", asOFFSET(noob::actor_blueprints, strength)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("actor_blueprints", "uint16 range", asOFFSET(noob::actor_blueprints, range)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("actor_blueprints", "uint16 armour", asOFFSET(noob::actor_blueprints, armour)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("actor_blueprints", "uint16 defense", asOFFSET(noob::actor_blueprints, defense)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("actor_blueprints", "uint16 movement_speed", asOFFSET(noob::actor_blueprints, movement_speed)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("actor_blueprints", "uint16 attack_speed", asOFFSET(noob::actor_blueprints, attack_speed)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("actor_blueprints", "uint16 stamina", asOFFSET(noob::actor_blueprints, stamina)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("actor_blueprints", "uint16 morale", asOFFSET(noob::actor_blueprints, morale)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("actor_blueprints", "uint16 los", asOFFSET(noob::actor_blueprints, los)); assert(r >= 0);	

	r = script_engine->RegisterObjectProperty("actor_blueprints", "shape_handle bounds", asOFFSET(noob::actor_blueprints, bounds)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("actor_blueprints", "shader_handle shader", asOFFSET(noob::actor_blueprints, shader)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("actor_blueprints", "reflectance_handle reflect", asOFFSET(noob::actor_blueprints, reflect)); assert(r >= 0);	
}
