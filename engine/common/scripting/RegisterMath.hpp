#pragma once

#include "MathFuncs.hpp"

#include <angelscript.h>
#include "AngelVector.hpp"

namespace noob
{
	void register_math(asIScriptEngine*);

	static void as_vec2_constructor_wrapper_basic(uint8_t* memory)
	{
		new(memory) noob::vec2();
	}

	static void as_vec2_constructor_wrapper_float_2(uint8_t* memory, float x, float y)
	{
		new(memory) noob::vec2(x, y);
	}

	static void as_vec2_destructor_wrapper(uint8_t* memory)
	{
		((noob::vec2*)memory)->~vec2();
	}

	static void as_vec3_constructor_wrapper_basic(uint8_t* memory)
	{
		new(memory) noob::vec3();
	}

	static void as_vec3_constructor_wrapper_float_3(uint8_t* memory, float x, float y, float z)
	{
		new(memory) noob::vec3(x, y, z);
	}

	static void as_vec3_constructor_wrapper_vec2_float(uint8_t* memory, const noob::vec2& vv,  float z)
	{
		new(memory) noob::vec3(vv, z);
	}

	static void as_vec3_constructor_wrapper_vec3(uint8_t* memory, const noob::vec3& vv)
	{
		new(memory) noob::vec3(vv);
	}

	static void as_vec3_destructor_wrapper(uint8_t* memory)
	{
		((noob::vec3*)memory)->~vec3();
	}

	static void as_vec4_constructor_wrapper_basic(uint8_t* memory)
	{
		new(memory) noob::vec4();
	}

	static void as_vec4_constructor_wrapper_float_4(uint8_t* memory, float x, float y, float z, float w)
	{
		new(memory) noob::vec4(x, y, z, w);
	}

	static void as_vec4_constructor_wrapper_vec2_float_2(uint8_t* memory, const noob::vec2& vv,  float z, float w)
	{
		new(memory) noob::vec4(vv, z, w);
	}

	static void as_vec4_constructor_wrapper_vec3_float(uint8_t* memory, const noob::vec3& vv, float w)
	{
		new(memory) noob::vec4(vv, w);
	}

	static void as_vec4_destructor_wrapper(uint8_t* memory)
	{
		((noob::vec4*)memory)->~vec4();
	}

	static void as_mat3_constructor_wrapper_basic(uint8_t* memory)
	{
		new(memory) noob::mat3();
	}

	static void as_mat3_constructor_wrapper_float_9(uint8_t* memory, float a, float b, float c, float d, float e, float f, float g, float h, float i)
	{
		new(memory) noob::mat3(a, b, c, d, e, f, g, h, i);
	}

	static void as_mat3_destructor_wrapper(uint8_t* memory)
	{
		((noob::mat3*)memory)->~mat3();
	}

	static void as_mat4_constructor_wrapper_basic(uint8_t* memory)
	{
		new(memory) noob::mat4();
	}

	static void as_mat4_constructor_wrapper_float_16(uint8_t* memory, float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float mm, float n, float o, float p)
	{
		new(memory) noob::mat4(a, b, c, d, e, f, g, h, i, j, k, l, mm, n, o, p);
	}

	static void as_mat4_destructor_wrapper(uint8_t* memory)
	{
		((noob::mat4*)memory)->~mat4();
	}

	static void as_versor_constructor_wrapper_basic(uint8_t* memory)
	{
		new(memory) noob::versor();
	}

	static void as_versor_constructor_wrapper_float_4(uint8_t* memory, float x, float y, float z, float w)
	{
		new(memory) noob::versor(x, y, z, w);
	}

	static void as_versor_destructor_wrapper(uint8_t* memory)
	{
		((noob::versor*)memory)->~versor();
	}
}
