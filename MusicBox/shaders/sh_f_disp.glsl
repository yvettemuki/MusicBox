#version 460

// time attribute
uniform float current_time;
uniform float prev_time;
uniform float delta_time;
// light attribute
uniform bool set_light;
uniform int light_type;
uniform vec3 light_pos;
uniform vec3 eye_pos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform vec3 La;
uniform vec3 Ld;
uniform vec3 Ls;
uniform float shininess;
uniform float alpha;
// texture
layout(binding = 0) uniform sampler2D tex;
layout(binding = 1) uniform samplerCube albedo_tex;
layout(binding = 3) uniform samplerCube normal_tex;
layout(binding = 4) uniform samplerCube roughness_tex;
uniform bool set_texture;
uniform int texture_type;
// pass
uniform int pass;
// in 
smooth in vec3 norm;  
smooth in vec2 texCoord;
smooth in vec3 cubeTexCoord;
smooth in vec3 pos_w;
smooth in vec3 nw;  // normal 
smooth in vec3 test;

// global variables for calculation
vec3 ka0, kd0, ks0;

// out color
layout(location = 0) out vec4 fragcolor;

void setupColorConfiguration()
{
	// Point light
	if (set_texture)
	{
		// if cube map
		vec4 texcolor = texture(albedo_tex, cubeTexCoord);
		ka0 = texcolor.rgb;
		kd0 = texcolor.rgb;

		if (texture_type == 4)
		{
			vec4 roughness = texture(roughness_tex, cubeTexCoord);
			ka0 += roughness.rgb;
			kd0 += roughness.rgb;
		}
	}
	else 
	{
		ka0 = ka;
		kd0 = kd;
	}

	ks0 = ks;
}

// Phong lighting
vec4 phongLighting()
{
	vec3 nw = normalize(nw);

	// normal map
	if (set_texture && (texture_type == 2 || texture_type == 4))
	{
		vec3 n = texture(normal_tex, cubeTexCoord).rgb;
		nw = normalize(n * 2.0 - 1.0);
	}

	// ambient light calculation
	vec3 ambient = ka0 * La;

	// diffuse light calculation
	float d = distance(pos_w, light_pos);
	float attenuation = 1.0 / pow(d, 2);
	vec3 lw = normalize(light_pos - pos_w); // light direction
	float diff_cos_theta = max(dot(nw, lw), 0);
	vec3 diffuse = kd0 * attenuation * diff_cos_theta * Ld;

	// specular light calculation
	vec3 vw = normalize(eye_pos - pos_w);
	vec3 rw = normalize(reflect(-lw, nw));
	float spec_cos_theta = pow(max(dot(vw, rw), 0), shininess);
	vec3 specular = ks0 * attenuation * spec_cos_theta * Ls;
	
	return vec4(ambient + diffuse + specular, alpha);

}


void main()
{
	setupColorConfiguration();
	
	if (set_light && pass == 1)
		fragcolor = phongLighting();
	else 
	{
		if (set_texture)
			fragcolor = vec4(texture(albedo_tex, cubeTexCoord).rgb, alpha);
		else 
			fragcolor = vec4(ka0, alpha);
	}
		
}
