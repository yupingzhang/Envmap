//cube
#version 400 core

// transformation matrices
layout(std140) uniform Matrices {
    mat4 modelViewMatrix, modelViewInverse;
    mat4 projectionMatrix, projectionInverse;
};

// per-vertex input
in vec3 vPosition;

out vec3 texcoords;

void main() {

	texcoords = vPosition;
    
    gl_Position = projectionMatrix * modelViewMatrix * vec4(vPosition.x*505, vPosition.y*505, vPosition.z*505, 1);
}