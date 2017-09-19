// shadertype=glsl
layout(location = SCENE_POSITION_ATTRIB_LOCATION)
in vec4 Position;

layout(location = SCENE_TEXCOORD_ATTRIB_LOCATION)
in vec2 TexCoord;
out vec2 TexCoordFrag;

layout(location = SCENE_NORMAL_ATTRIB_LOCATION)
in vec3 Normal;

in vec4 vertex_color;
out vec3 vert_pos;
out vec3 normal_interp;
out vec4 shadowmap_coord;
uniform mat4 ModelWorld;
uniform mat4 ModelViewProjection;
uniform mat3 Normal_ModelWorld;
uniform mat4 LightMatrix;

void main()
{
    //Set to MVP * P 
    gl_Position = ModelViewProjection * Position;
    
    //Pass vertex attributes to fragment shader
	TexCoordFrag = TexCoord;
	vec4 vertpos4 = ModelWorld * Position; //convert from model to world space
	vert_pos = vertpos4.xyz / vertpos4.w;
	normal_interp = Normal_ModelWorld * Normal;

	//Pass shadow map stuff
	shadowmap_coord = LightMatrix * Position;
}