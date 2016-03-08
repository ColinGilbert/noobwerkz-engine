#include "RegisterHandles.hpp"


void noob::register_handles(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("shape_handle", sizeof(noob::shapes_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::shapes_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("shape_handle", "uint64 inner", asOFFSET(noob::shapes_holder::handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("body_handle", sizeof(noob::bodies_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::bodies_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("body_handle", "uint64 inner", asOFFSET(noob::bodies_holder::handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("model_handle", sizeof(noob::basic_models_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::basic_models_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("model_handle", "uint64 inner", asOFFSET(noob::basic_models_holder::handle, inner)); assert(r >= 0);

	r = script_engine->RegisterObjectType("animated_model_handle", sizeof(noob::animated_models_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::animated_models_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("animated_model_handle", "uint64 inner", asOFFSET(noob::animated_models_holder::handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("skeletal_anim_handle", sizeof(noob::skeletal_anims_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::skeletal_anims_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("skeletal_anim_handle", "uint64 inner", asOFFSET(noob::skeletal_anims_holder::handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("light_handle", sizeof(noob::lights_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::lights_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("light_handle", "uint64 inner", asOFFSET(noob::lights_holder::handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("reflectance_handle", sizeof(noob::reflectances_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::reflectances_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("reflectance_handle", "uint64 inner", asOFFSET(noob::reflectances_holder::handle, inner)); assert(r >= 0);
	
	r = script_engine->RegisterObjectType("shader_handle", sizeof(noob::shaders_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::shaders_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	// r = script_engine->RegisterObjectProperty("shader_handle", "uint64 inner", asOFFSET(noob::shaders_holder::handle, inner)); assert(r >= 0);
}
