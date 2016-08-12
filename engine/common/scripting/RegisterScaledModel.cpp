#include "RegisterScaledModel.hpp"


void noob::register_scaled_model(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("scaled_model", sizeof(noob::scaled_model), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::scaled_model>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	r = script_engine->RegisterObjectBehaviour("scaled_model", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_scaled_model_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("scaled_model", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_scaled_model_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("scaled_model", "model_handle model_h", asOFFSET(noob::scaled_model, model_h)); assert( r >= 0 );
	// r = script_engine->RegisterObjectProperty("scaled_model", "reflectance_handle reflect_h", asOFFSET(noob::scaled_model, reflect_h)); assert( r >= 0 );		
	r = script_engine->RegisterObjectProperty("scaled_model", "vec3 scales", asOFFSET(noob::scaled_model, scales)); assert( r >= 0 );
}
