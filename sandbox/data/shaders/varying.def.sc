vec3 v_position : POSITION = vec3(0.0, 0.0, 1.0);
vec3 v_normal : NORMAL = vec3(0.0, 0.0, 1.0);
// vec3 to_light : POSITION = vec3(0.0, 0.0, 1.0);
// vec3 to_eye : POSITION = vec3(0.0, 0.0, 1.0);
vec3 v_texcoord : TEXCOORD0 = vec3(1.0, 1.0, 1.0);
vec3 world_pos : TEXCOORD1 = vec3(0.0, 0.0, 0.0);
vec3 world_normal : TEXCOORD2 = vec3(0.0, 0.0, 1.0);
vec3 mult_normal : TEXCOORD3 =  vec3(0.0, 0.0, 0.0);
vec3 world_eye : TEXCOORD4 = vec3(0.0, 0.0, 1.0);

vec3 a_position : POSITION;
vec3 a_normal : NORMAL;
vec3 a_texcoord : TEXCOORD0;
