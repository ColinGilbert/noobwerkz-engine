// Shader uniform blocks. This is to get away from the machinery that uses boost::variant and some of the crazy namespacing and contortionism that comes up far too often when writing client-side code.
#pragma once

namespace noob
{
	
	enum shader_type
	{
		BASIC = 0,
		TRIPLANAR = 1 << 1,
	};

	class basic_shader_uniform
	{
		
	};

	class triplanar_shader_uniform
	{

	};
}
