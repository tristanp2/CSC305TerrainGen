in vec3 texture_coords;
out vec4 out_color;

uniform samplerCube CubeMap;

void main(void){
    out_color = texture(CubeMap, texture_coords);
}