// shadertype=glsl

uniform vec3 CameraPos;

uniform vec3 Ambient;
uniform vec3 Diffuse;
uniform vec3 Specular;
uniform float Shininess;

uniform int HasDiffuseMap;
uniform sampler2D DiffuseMap;
uniform int IsMultiSampled;
uniform sampler2D SecondaryMap;
uniform int TiledTexture;
uniform sampler2DShadow ShadowMap;
//uniform sampler2D ShadowMap;

in vec2 TexCoordFrag;
out vec4 FragColor;
in vec3 vert_pos;
in vec3 normal_interp;
in vec4 shadowmap_coord;

void main()
{
	vec3 diffuse_map, secondary_map;
	vec2 tex_coord;
	vec3 normal = normalize(normal_interp);
	float mix_min = 0.0, mix_max = 3.0;
	mix_min -= 20.0;
	mix_max -= 20.0;
	float secondary_factor = 0.0, primary_factor = 1.0;
	if(TiledTexture != 0){
		tex_coord = TexCoordFrag * 15;
	}
	else{
		tex_coord = TexCoordFrag;
	}

	if(HasDiffuseMap != 0){
		diffuse_map = texture(DiffuseMap, tex_coord).rgb;
	}
	else{
		diffuse_map = vec3(1.0);
	}
	float delta_y = 0;;
	if(IsMultiSampled != 0){
		secondary_map = texture(SecondaryMap, tex_coord).rgb;
		if(vert_pos.y > mix_min){
			if(vert_pos.y < mix_max){
				delta_y = vert_pos.y - mix_min;
				secondary_factor = delta_y / (mix_max - mix_min);
				primary_factor = 1 - secondary_factor;
			}
			else{
				secondary_factor = 1.0;
				primary_factor = 0.0;
			}
		}
		else{
			secondary_factor = 0.0;
			primary_factor = 1.0;
		}
	}
	else{
		secondary_map = vec3(1.0);
		secondary_factor = 0.0;
	}
	vec3 light_dir = normalize(CameraPos - vert_pos);
	float lambertian = max(dot(light_dir, normal), 0.0); //should also be specangle?
	float specular = 0;
	vec3 color_linear;
	if(lambertian > 0){
		specular = pow(lambertian, Shininess);	
	}

	float visibility = textureProj(ShadowMap, shadowmap_coord);
	 color_linear = lambertian * (diffuse_map * primary_factor + secondary_map * secondary_factor) + specular * Specular;
	vec3 testy = vec3(normal.y - (normal.x + normal.z));
	vec3 gamma_corr = pow(color_linear, vec3(1.0 / 2.2));
	FragColor = vec4(gamma_corr * visibility + Ambient*gamma_corr*0.1, 1.0);
	//FragColor = vec4(3*normal/4, 1.0);
}