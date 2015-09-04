/*
vec2 blinn(vec3 light_direction, vec3 normal, vec3 view_direction)
{
	float n_dot_l = dot(normal, light_direction);
	vec3 reflected = light_direction - 2.0 * n_dot_l * normal; // reflect(light_direction, normal);
	float r_dot_v = dot(reflected, view_direction);
	return vec2(n_dot_l, r_dot_v);
}

float fresnel(float n_dot_l, float bias, float pow)
{
	float facing =(1.0 - n_dot_l);
	return max(bias +(1.0 - bias) * pow(facing, pow), 0.0);
}

vec4 lit(float n_dot_l, float r_dot_v, float m)
{
	float diff = max(0.0, n_dot_l);
	float spec = step(0.0, n_dot_l) * max(0.0, r_dot_v * m);
	return vec4(1.0, diff, spec, 1.0);
}

// Usage(fragment)
void main()
{
	vec3 light_dir = vec3(0.0, 0.0, -1.0);
	vec3 normal = normalize(v_normal);
	vec3 view = normalize(v_view);
	vec2 bln = blinn(light_dir, normal, view);
	vec4 lc = lit(bln.x, bln.y, 1.0);
	float fres = fresnel(bln.x, 0.2, 5.0);

	float index =((sin(v_pos.x * 3.0 + u_time.x) * 0.3 + 0.7) +(cos(v_pos.y * 3.0 + u_time.x) * 0.4 + 0.6) +(cos( v_pos.z * 3.0 + u_time.x) * 0.2 + 0.8)) * M_PI;
	vec3 color = vec3(sin(index * 8.0) * 0.4 + 0.6, sin(index * 4.0) * 0.4 + 0.6, sin(index * 2.0) * 0.4 + 0.6) * v_color0.xyz;

	gl_FragColor.xyz = pow(vec3(0.07, 0.06, 0.08) + color * lc.y + fres * pow(lc.z, 128.0), vec3_splat(1.0 / 2.2));
	gl_FragColor.w = 1.0;
}
*/

vec4 blinn_phong(vec3 light_world_pos, vec3 light_specular, vec3 light_diffuse, vec3 light_ambient, vec3 surface_specular, vec3 surface_diffuse, vec3 surface_ambient, float specular_power)
{
	// ambient intensity
	vec3 ambient_intensity = light_ambient * surface_ambient;

	// diffuse intensity
	// raise light position to eye space
	vec3 light_position_eye = vec3(view_mat * vec4(light_world_pos, 1.0));
	vec3 distance_to_light_eye = light_position_eye - position_eye;
	vec3 direction_to_light_eye = normalize(distance_to_light_eye);
	float dot_prod = dot(direction_to_light_eye, normal_eye);
	dot_prod = max(dot_prod, 0.0);
	vec3 Id = light_diffuse * surface_diffuse * dot_prod; // final diffuse intensity
	
	// specular intensity
	vec3 surface_to_viewer_eye = normalize(-position_eye);
	
	//vec3 reflection_eye = reflect(-direction_to_light_eye, normal_eye);
	//float dot_prod_specular = dot(reflection_eye, surface_to_viewer_eye);
	//dot_prod_specular = max(dot_prod_specular, 0.0);
	//float specular_factor = pow(dot_prod_specular, specular_power);
	
	// blinn
	vec3 half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
	float dot_prod_specular = max(dot(half_way_eye, normal_eye), 0.0);
	float specular_factor = pow(dot_prod_specular, specular_power);
	
	vec3 Is = light_specular * surface_specular * specular_factor; // final specular intensity
	
	// final colour
	return vec4(Is + Id + ambient_intensity, 1.0);
}
