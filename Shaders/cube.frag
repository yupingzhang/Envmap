// fragment shader for the cube room
#version 400 core

uniform samplerCube cubeTexture;
in vec3 texcoords;

out vec4 fragColor;

void main() {
	 
	fragColor = texture(cubeTexture, texcoords); 
}




