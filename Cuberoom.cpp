//
//  Cuberoom.cpp
//  Aassn3
//
//  Created by Yuping on 3/25/14.
//  Copyright (c) 2014 Yuping. All rights reserved.
//


#include "Cuberoom.hpp"
#include "AppContext.hpp"
#include "Vec.inl"

// using core modern OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define FRONT "RomeChurch/negz.jpg"
#define BACK "RomeChurch/posz.jpg"
#define TOP "RomeChurch/posy.jpg"
#define BOTTOM "RomeChurch/negy.jpg"
#define LEFT "RomeChurch/negx.jpg"
#define RIGHT "RomeChurch/posx.jpg"


Cuberoom::Cuberoom()
{
    // buffer objects to be used later
    //glGenTextures(1, textureID);
    glGenBuffers(NUM_BUFFERS, bufferIDs);
    glGenVertexArrays(NUM_VARRAYS, varrayIDs);
    
    //updateTexture(textureIDs[CUBE_TEXTURE]);
    updateTexture();
    
    // build vertex array
    numvert = sizeof(vert)/sizeof(*vert);
    vert[0] = vec3<float>( -1.f, 1.f, -1.f);
    vert[1] = vec3<float>( -1.f, -1.f, -1.f);
    vert[2] = vec3<float>( 1.f, -1.f, -1.f);
    vert[3] = vec3<float>( 1.f, 1.f, -1.f);
    vert[4] = vec3<float>( -1.f, 1.f, 1.f);
    vert[5] = vec3<float>( -1.f, -1.f, 1.f);
    vert[6] = vec3<float>( 1.f, -1.f, 1.f);
    vert[7] = vec3<float>( 1.f, 1.f, 1.f);
    
    // build index array linking sets of three vertices into triangles
    numtri = sizeof(indices)/sizeof(*indices);
    indices[0] = vec3<unsigned int>(0, 1, 2);
    indices[1] = vec3<unsigned int>(0, 2, 3);
    indices[2] = vec3<unsigned int>(3, 2, 6);
    indices[3] = vec3<unsigned int>(3, 6, 7);
    indices[4] = vec3<unsigned int>(7, 6, 5);
    indices[5] = vec3<unsigned int>(7, 5, 4);
    indices[6] = vec3<unsigned int>(4, 5, 1);
    indices[7] = vec3<unsigned int>(4, 1, 0);
    indices[8] = vec3<unsigned int>(4, 0, 3);
    indices[9] = vec3<unsigned int>(4, 3, 7);
    indices[10] = vec3<unsigned int>(6, 2, 1);
    indices[11] = vec3<unsigned int>(6, 1, 5);
    
    
    // load vertex and index array to GPU
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, numvert*sizeof(Vec3f), vert, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 numtri*sizeof(unsigned int[3]), indices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // initial shader load
    shaderParts[0].id = glCreateShader(GL_VERTEX_SHADER);
    shaderParts[0].file = "cube.vert";
    shaderParts[1].id = glCreateShader(GL_FRAGMENT_SHADER);
    shaderParts[1].file = "cube.frag";
    shaderID = glCreateProgram();
    updateShaders();
    
}

Cuberoom::~Cuberoom()
{
    glDeleteProgram(shaderID);
    glDeleteBuffers(NUM_BUFFERS, bufferIDs);
    //glDeleteTextures(NUM_TEXTURES, textureIDs);
}

bool Cuberoom::updateTexture()
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    

    Magick::Image* pImage = NULL;
    Magick::Blob blob;
    
    std::string fileNames[6] = { RIGHT, LEFT, TOP, BOTTOM, BACK, FRONT};
    GLenum faces[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
    for (unsigned int i = 0 ; i < 6; i++) {
        pImage = new Magick::Image(fileNames[i]);
        try {
            pImage->write(&blob, "RGBA");
        }
        catch (Magick::Error& Error) {
            std::cout << "Error loading texture '" << fileNames[i] << "': " << Error.what() << std::endl;
            delete pImage;
            return false;
        }
        
        glTexImage2D(faces[i], 0, GL_RGB, pImage->columns(), pImage->rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());
        delete pImage;
    }
    
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return true;
}

void Cuberoom::updateShaders()
{
    loadShaders(shaderID, sizeof(shaderParts)/sizeof(*shaderParts),
                shaderParts);
    glUseProgram(shaderID);
    
    // (re)connect view and projection matrices
    glUniformBlockBinding(shaderID,
                          glGetUniformBlockIndex(shaderID,"Matrices"),
                          AppContext::MATRIX_UNIFORMS);
    
    glUniform1i(glGetUniformLocation(shaderID, "cubeTexture"), textureID);
    // re-connect attribute arrays
    glBindVertexArray(varrayIDs[CUBE_VARRAY]);
    
    positionAttrib = glGetAttribLocation(shaderID, "vPosition");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttrib);
    
    // turn off everything we enabled
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);

}

void Cuberoom::draw() const
{
    // enable shaders
    glUseProgram(shaderID);
    glActiveTexture(GL_TEXTURE0 + textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    // enable vertex arrays
    glBindVertexArray(varrayIDs[CUBE_VARRAY]);
    
    // draw the triangles for each three indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glDrawElements(GL_TRIANGLES, 3*numtri, GL_UNSIGNED_INT, 0);
    
    // turn of whatever we turned on
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUseProgram(0);
}
