#include "RegisterMath.hpp"


void noob::register_math(asIScriptEngine* script_engine)
{
	int r;

	RegisterVector<float>("float", script_engine);
	RegisterVector<double>("double", script_engine);
	RegisterVector<int>("int", script_engine);
	RegisterVector<unsigned int>("uint", script_engine);

	r = script_engine->RegisterEnum("csg_op"); assert(r >= 0 );
	r = script_engine->RegisterEnumValue("csg_op", "UNION", 0); assert(r >= 0 );
	r = script_engine->RegisterEnumValue("csg_op", "DIFFERENCE", 1); assert(r >= 0 );
	r = script_engine->RegisterEnumValue("csg_op", "INTERSECTION", 2); assert(r >= 0 );

	r = script_engine->RegisterObjectType("vec2", sizeof(vec2), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::vec2>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::vec2>("vec2", script_engine);
	r = script_engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(as_vec2_constructor_wrapper_basic), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(as_vec2_constructor_wrapper_float_2), asCALL_CDECL_OBJLAST); assert( r >= 0);
	r = script_engine->RegisterObjectBehaviour("vec2", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_vec2_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("vec2", "void set_opIndex(int, float)", asMETHOD(noob::vec2, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec2", "float get_opIndex(int) const", asMETHOD(noob::vec2, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("vec3", sizeof(vec3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::vec3>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::vec3>("vec3", script_engine);
	r = script_engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(as_vec3_constructor_wrapper_basic), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(as_vec3_constructor_wrapper_float_3), asCALL_CDECL_OBJLAST); assert( r >= 0);
	r = script_engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(vec2, float)", asFUNCTION(as_vec3_constructor_wrapper_vec2_float), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("vec3", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_vec3_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opAdd(float) const", asMETHODPR(noob::vec3, operator+, (float) const, noob::vec3), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opAdd(const vec3& in) const", asMETHODPR(noob::vec3, operator+, (const noob::vec3&) const, noob::vec3), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec3", "vec3& opAddAssign(const vec3& in)", asMETHOD(noob::vec3, operator+=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opSub(const vec3& in) const", asMETHODPR(noob::vec3, operator-, (float) const, noob::vec3), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opSub(float) const", asMETHODPR(noob::vec3, operator-, (const noob::vec3&) const, noob::vec3), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3& opSubAssign(const vec3& in)", asMETHOD(noob::vec3, operator-=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opMult(float)", asMETHOD(noob::vec3, operator*), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3& opMultAssign(float)", asMETHOD(noob::vec3, operator*=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opDiv(float)", asMETHOD(noob::vec3, operator/), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3& opAssign(const vec3& in)", asMETHOD(noob::vec3, operator=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "void set_opIndex(uint32, float)", asMETHOD(noob::vec3, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec3", "float get_opIndex(uint32) const", asMETHOD(noob::vec3, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("vec4", sizeof(vec4), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::vec4>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::vec4>("vec4", script_engine);
	r = script_engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(as_vec4_constructor_wrapper_basic), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(as_vec4_constructor_wrapper_float_4), asCALL_CDECL_OBJLAST); assert( r >= 0);
	r = script_engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(vec2, float, float)", asFUNCTION(as_vec4_constructor_wrapper_vec2_float_2), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(vec3, float)", asFUNCTION(as_vec4_constructor_wrapper_vec3_float), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("vec4", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_vec4_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("vec4", "vec4 opAdd(float) const", asMETHODPR(noob::vec4, operator+, (float) const, noob::vec4), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec4", "vec4 opAdd(const vec4& in) const", asMETHODPR(noob::vec4, operator+, (const noob::vec4&) const, noob::vec4), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec4", "vec4& opAddAssign(const vec4& in)", asMETHOD(noob::vec4, operator+=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec4", "vec4 opSub(const vec4& in) const", asMETHODPR(noob::vec4, operator-, (float) const, noob::vec4), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec4", "vec4 opSub(float) const", asMETHODPR(noob::vec4, operator-, (const noob::vec4&) const, noob::vec4), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec4", "vec4& opSubAssign(const vec4& in)", asMETHOD(noob::vec4, operator-=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec4", "vec4 opMult(float)", asMETHOD(noob::vec4, operator*), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec4", "vec4& opMultAssign(float)", asMETHOD(noob::vec4, operator*=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec4", "vec4 opDiv(float)", asMETHOD(noob::vec4, operator/), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec4", "vec4& opAssign(const vec4& in)", asMETHOD(noob::vec4, operator=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec4", "void set_opIndex(uint32, float)", asMETHOD(noob::vec4, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec4", "float get_opIndex(uint32) const", asMETHOD(noob::vec4, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("mat3", sizeof(mat3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::mat3>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::mat3>("mat3", script_engine);

	r = script_engine->RegisterObjectType("mat4", sizeof(mat4), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::mat4>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::mat4>("mat4", script_engine);
	r = script_engine->RegisterObjectMethod("mat4", "mat4 opMult(const vec4& in) const", asMETHODPR(noob::mat4, operator*, (const noob::vec4&) const, noob::vec4), asCALL_THISCALL); assert (r >= 0);
	r = script_engine->RegisterObjectMethod("mat4", "mat4 opMult(const mat4& in) const", asMETHODPR(noob::mat4, operator*, (const noob::mat4&) const, noob::mat4), asCALL_THISCALL); assert (r >= 0);
	r = script_engine->RegisterObjectMethod("mat4", "void set_opIndex(uint32, float)", asMETHOD(noob::mat4, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("mat4", "float get_opIndex(uint32) const", asMETHOD(noob::mat4, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("versor", sizeof(versor), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::versor>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	RegisterVector<noob::versor>("versor", script_engine);
	r = script_engine->RegisterObjectBehaviour("versor", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(as_versor_constructor_wrapper_basic), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("versor", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(as_versor_constructor_wrapper_float_4), asCALL_CDECL_OBJLAST); assert( r >= 0);
	r = script_engine->RegisterObjectBehaviour("versor", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_versor_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("versor", "vec3 opDiv(float) const", asMETHOD(noob::versor, operator/), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "versor opMult(float) const", asMETHODPR(noob::versor, operator*, (float) const, noob::versor), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "versor opMult(const versor& in) const", asMETHODPR(noob::versor, operator*, (const noob::versor&) const, noob::versor), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "versor& opAssign(const versor& in)", asMETHOD(noob::versor, operator=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "void set_opIndex(uint32, float)", asMETHOD(noob::versor, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("versor", "float get_opIndex(uint32) const", asMETHOD(noob::versor, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("bbox", sizeof(cubic_region), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::cubic_region>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::cubic_region>("bbox", script_engine);
	r = script_engine->RegisterObjectProperty("bbox", "vec3 min", asOFFSET(noob::bbox, min)); assert (r >= 0);
	r = script_engine->RegisterObjectProperty("bbox", "vec3 max", asOFFSET(noob::bbox, max)); assert (r >= 0);
	r = script_engine->RegisterObjectMethod("bbox", "vec3 get_dims() const", asMETHOD(noob::bbox, get_dims), asCALL_THISCALL); assert(r >= 0 );
	
	r = script_engine->RegisterObjectType("cubic_region", sizeof(cubic_region), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::cubic_region>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::cubic_region>("cubic_region", script_engine);
	r = script_engine->RegisterObjectProperty("cubic_region", "vec3 lower_corner", asOFFSET(noob::cubic_region, lower_corner)); assert (r >= 0);
	r = script_engine->RegisterObjectProperty("cubic_region", "vec3 upper_corner", asOFFSET(noob::cubic_region, upper_corner)); assert (r >= 0);

	r = script_engine->RegisterGlobalFunction("bool compare_floats(float, float)", asFUNCTION(compare_floats), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float length(const vec3& in)", asFUNCTION(length_squared), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float length_squared(const vec3& in)", asFUNCTION(length), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("vec3 normalize(const vec3& in)", asFUNCTIONPR(normalize, (const vec3&), vec3), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float dot(const vec3& in, const vec3& in)", asFUNCTIONPR(dot, (const vec3&, const vec3&), float), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("vec3 cross(const vec3& in, const vec3& in)", asFUNCTION(cross), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float get_squared_dist(const vec3& in, const vec3& in)", asFUNCTION(get_squared_dist), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float direction_to_heading(const vec3& in)", asFUNCTION(direction_to_heading), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("vec3 heading_to_direction(float)", asFUNCTION(heading_to_direction), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("bool linearly_dependent(const vec3& in, const vec3& in, const vec3& in)", asFUNCTION(linearly_dependent), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat3 zero_mat3()", asFUNCTION(zero_mat3), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat3 identity_mat3()", asFUNCTION(identity_mat3), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 zero_mat4()", asFUNCTION(zero_mat4), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 identity_mat4()", asFUNCTION(identity_mat4), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("float determinant(const mat4& in)", asFUNCTION(determinant), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 inverse(const mat4& in)", asFUNCTION(inverse), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 transpose(const mat4& in)", asFUNCTION(transpose), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 translate(const mat4& in, const vec3& in)", asFUNCTION(translate), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 rotate_x_deg(const mat4& in, float)", asFUNCTION(rotate_x_deg), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 rotate_y_deg(const mat4& in, float)", asFUNCTION(rotate_y_deg), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 rotate_z_deg(const mat4& in, float)", asFUNCTION(rotate_z_deg), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 scale(const mat4& in, const vec3& in)", asFUNCTION(scale), asCALL_CDECL); assert(r >= 0);
	// vec3 translation_from_mat4(const mat4& m);
	// vec3 scale_from_mat4(const mat4& m);
	r = script_engine->RegisterGlobalFunction("mat4 look_at(const vec3& in, const vec3& in, const vec3& in)", asFUNCTION(look_at), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 perspective(float, float, float, float)", asFUNCTION(perspective), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 scale(float, float, float, float, float, float)", asFUNCTION(ortho), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor quat_from_axis_rad(float, float, float, float)", asFUNCTION(quat_from_axis_rad), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor quat_from_axis_deg(float, float, float, float)", asFUNCTION(quat_from_axis_deg), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor quat_from_mat4(const mat4& in)", asFUNCTION(quat_from_mat4), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 quat_to_mat4(const versor& in)", asFUNCTION(quat_to_mat4), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("float dot(const versor& in, const versor& in)", asFUNCTIONPR(dot, (const versor&, const versor&), float), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor normalize(const versor& in)", asFUNCTIONPR(normalize, (const versor&), versor), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor slerp(const versor& in, const versor& in, float)", asFUNCTION(slerp), asCALL_CDECL); assert(r >= 0);
}
