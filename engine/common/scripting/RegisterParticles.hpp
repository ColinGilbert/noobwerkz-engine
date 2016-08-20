#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"
#include "RegisterGlobals.hpp"

namespace noob
{
	void register_particles(asIScriptEngine*);


	// TODO: Not necessary?
	/*
	   static void as_particle_constructor_wrapper_basic(uint8_t* memory)
	   {
	   new(memory) noob::particle();
	   }

	   static void as_particle_destructor_wrapper(uint8_t* memory)
	   {
	   ((noob::particle*)memory)->~particle();
	   }

	   static void as_particle_system_descriptor_constructor_wrapper_basic(uint8_t* memory)
	   {
	   new(memory) noob::particle_system_descriptor();
	   }

	   static void as_particle_system_descriptor_destructor_wrapper(uint8_t* memory)
	   {
	   ((noob::particle_system_descriptor*)memory)->~particle_system_descriptor();
	   }
	   */
}
