//look up in the cubemap texture
//find the reflect color

#version 400 core

in vec3 ePosition, eNormal;
in mat4 inverseViewMatrix;
uniform samplerCube cubeTexture;
out vec4 fragColor;

void main () {

    // reflect ray around normal from eye to surface 
    vec3 pos = normalize(ePosition);
    vec3 nor = normalize(eNormal);
    vec3 reflected = reflect(pos, nor);
    reflected = vec3(inverseViewMatrix * vec4 (reflected, 0.0));

    fragColor = texture(cubeTexture, reflected);
	 
}

