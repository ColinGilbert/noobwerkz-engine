$input a_position, a_color0
$output v_color0

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <bgfx_shader.sh>

void main()
{
	gl_Position = mul(u_viewProj, vec4(a_position.xy, 0.0, 1.0) );
	v_color0 = a_color0;
}
