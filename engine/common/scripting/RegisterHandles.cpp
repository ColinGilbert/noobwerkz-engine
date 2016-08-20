#include "RegisterHandles.hpp"

#include "ComponentDefines.hpp"
#include "NoobCommon.hpp"
#include "Actor.hpp"
#include "Particles.hpp"

void noob::register_handles(asIScriptEngine* script_engine)
{
	int r;

	r = script_engine->RegisterObjectType("shape_handle", sizeof(noob::shape_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::shape_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	
	r = script_engine->RegisterObjectType("body_handle", sizeof(noob::body_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::body_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);

	r = script_engine->RegisterObjectType("ghost_handle", sizeof(noob::ghost_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::ghost_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);

	r = script_engine->RegisterObjectType("joint_handle", sizeof(noob::joint_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::joint_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);

	r = script_engine->RegisterObjectType("model_handle", sizeof(noob::model_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::model_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);

	r = script_engine->RegisterObjectType("animated_model_handle", sizeof(noob::animated_model_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::animated_model_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	
	r = script_engine->RegisterObjectType("skeletal_anim_handle", sizeof(noob::skeletal_anim_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::skeletal_anim_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);

	r = script_engine->RegisterObjectType("actor_blueprints_handle", sizeof(noob::actor_blueprints_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::actor_blueprints_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);

	r = script_engine->RegisterObjectType("actor_handle", sizeof(noob::actor_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::actor_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);

	r = script_engine->RegisterObjectType("scenery_handle", sizeof(noob::scenery_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::scenery_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);

	r = script_engine->RegisterObjectType("particle_system_handle", sizeof(noob::particle_system_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::particle_system_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);

	r = script_engine->RegisterObjectType("light_handle", sizeof(noob::light_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::light_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	
	r = script_engine->RegisterObjectType("reflectance_handle", sizeof(noob::reflectance_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::reflectance_handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	
	r = script_engine->RegisterObjectType("shader_handle", sizeof(noob::shader), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::shader>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
}
