// fragment shader for the cube room
#version 400 core

uniform samplerCube cubeTexture;
in vec3 texcoords;

out vec4 fragColor;

void main() {
	
	//fragColor = vec4(texcoords, 1.0);

    fragColor = texture(cubeTexture, texcoords); 

}




