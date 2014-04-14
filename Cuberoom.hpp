//
//  Cuberoom.h
//  Aassn3
//
//  Created by Yuping on 3/25/14.
//  Copyright (c) 2014 Yuping. All rights reserved.
//

#ifndef __Aassn3__Cuberoom__
#define __Aassn3__Cuberoom__

#include <iostream>
#include "Vec.hpp"
#include "Shader.hpp"
#include "Magick++.h"
#include <cstring>

// using core modern OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Cuberoom {
private:
    unsigned int numvert;       // total vertices
    Vec3f vert[8];              // per-vertex position
    
    unsigned int numtri;             // total triangles
    Vec<unsigned int, 3> indices[12]; // 3 vertex indices per triangle
    
    // GL vertex array object IDs
    enum {CUBE_VARRAY, NUM_VARRAYS};
    unsigned int varrayIDs[NUM_VARRAYS];
    
    // GL buffer object IDs
    enum {POSITION_BUFFER, INDEX_BUFFER, NUM_BUFFERS};
    unsigned int bufferIDs[NUM_BUFFERS];
    
    // GL shaders
    unsigned int shaderID;      // ID for shader program
    ShaderInfo shaderParts[2];  // vertex & fragment shader info
    
    // vertex attribute IDs
    unsigned int positionAttrib;
    
public:
    //textures
    unsigned int textureID, depthbuffer;
  
    // public methods
public:
    
    Cuberoom();
    ~Cuberoom();
    // load/reload a texture
    //void updateTexture();
    bool updateTexture();
    
    // load/reload shaders
    void updateShaders();
    
    void draw() const;

};


#endif /* defined(__Aassn3__Cuberoom__) */
