#include "RegisterHandles.hpp"

void noob::register_handles(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("shape_handle", sizeof(noob::shape_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::shape_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("shape_handle", "uint64 inner", asOFFSET(noob::shape_handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("body_handle", sizeof(noob::body_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::body_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("body_handle", "uint64 inner", asOFFSET(noob::body_handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("model_handle", sizeof(noob::model_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::model_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("model_handle", "uint64 inner", asOFFSET(noob::model_handle, inner)); assert(r >= 0);

	r = script_engine->RegisterObjectType("animated_model_handle", sizeof(noob::animated_model_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::animated_model_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("animated_model_handle", "uint64 inner", asOFFSET(noob::animated_model_handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("skeletal_anim_handle", sizeof(noob::skeletal_anim_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::skeletal_anim_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("skeletal_anim_handle", "uint64 inner", asOFFSET(noob::skeletal_anim_handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("light_handle", sizeof(noob::light_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::light_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("light_handle", "uint64 inner", asOFFSET(noob::light_handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("reflectance_handle", sizeof(noob::reflectance_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::reflectance_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("reflectance_handle", "uint64 inner", asOFFSET(noob::reflectance_handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("shader_handle", sizeof(noob::globals::shader_results), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::globals::shader_results>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("shader_handle", "uint64 inner", asOFFSET(noob::shader_handle, inner)); assert(r >= 0);
}
