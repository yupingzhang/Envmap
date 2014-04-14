//look up in the cubemap texture
//find the reflect color

#version 400 core

in vec3 ePosition, eNomarl;
in mat4 inverseViewMatrix;
uniform samplerCube cubeTexture;
out vec4 fragColor;

void main () {

    // reflect ray around normal from eye to surface 
    vec3 reflected = reflect(ePosition, eNomarl);
    reflected = normalize(vec3(inverseViewMatrix * vec4 (reflected, 0.0)));

    fragColor = texture(cubeTexture, reflected);
	//fragColor = vec4(0., 0.5, 0.9, 1.0);
}

