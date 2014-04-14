//cubemap
#version 400 core

// transformation matrices
layout(std140) uniform Matrices {
    mat4 modelViewMatrix, modelViewInverse;
    mat4 projectionMatrix, projectionInverse;
    vec3 objectpos;
};

// per-vertex input
in vec3 vPosition;
in vec3 vNormal;

out vec3 ePosition, eNomarl;
out mat4 inverseViewMatrix;

void main() {
	
    //in eye space
	ePosition = vec3(modelViewMatrix * vec4(vPosition, 1.0));
	eNomarl = vec3(modelViewMatrix * vec4(vNormal, 0.0));
	inverseViewMatrix = modelViewInverse;

    vec3 newPos = vPosition + objectpos;
    gl_Position = projectionMatrix * modelViewMatrix * vec4(newPos, 1);       
    //gl_Position = projectionMatrix * modelViewMatrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1);
}
