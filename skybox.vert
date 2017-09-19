layout(location = SKYBOX_POSITION_ATTRIB_LOCATION)
in vec3 position;

out vec3 texture_coords;

uniform mat4 WorldView;

void main(void){
	
	gl_Position = WorldView * vec4(position, 1.0); 
	texture_coords = position;	
}