//cubemap
#version 400 core

// transformation matrices
layout(std140) uniform Matrices {
    mat4 modelViewMatrix, modelViewInverse;
    mat4 projectionMatrix, projectionInverse;
    vec3 lightpos;
};

// per-vertex input
in vec3 vPosition;
in vec3 vNormal;

out vec3 position, fNomarl, light;
out mat4 inverseViewMatrix;

void main() {
	
    //in eye space
	position = vec3(modelViewMatrix * vec4(vPosition, 1.0));
	fNomarl = vec3(modelViewMatrix * vec4(vNormal, 0.0));
	inverseViewMatrix = modelViewInverse;
    light = vec3(modelViewMatrix * vec4(lightpos, 1.0));
    
    gl_Position = projectionMatrix * modelViewMatrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1);
}
