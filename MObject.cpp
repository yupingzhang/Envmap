//draw reflexible object

#include "AppContext.hpp"
#include "MObject.hpp"
#include "Vec.inl"

// using core modern OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>


MObject::MObject() {
    
    // buffer objects
    glGenBuffers(NUM_BUFFERS, bufferIDs);
    glGenVertexArrays(NUM_VARRAYS, varrayIDs);

    iFactor = 100;
    iRadius = 15;
    vert.resize(iFactor*iFactor*3);
    normal.resize(iFactor*iFactor*3);
    indices.resize(iFactor*iFactor*6);
    
    float phi, theta;
    std::vector<GLfloat>::iterator v = vert.begin();
    std::vector<GLfloat>::iterator n = normal.begin();
    for (int i = 0; i<iFactor; i++) {
        theta = (M_PI*i)/iFactor;
        
        for (int j = 0; j<iFactor; j++) {
            phi = (2*M_PI*j)/iFactor;
            
            *v++ = (float)(sin(theta)*cos(phi)*iRadius);
            *v++ = (float)(sin(theta)*sin(phi)*iRadius);
            *v++ = (float)(cos(theta)*iRadius);
            
            *n++ = (float)(sin(theta)*cos(phi));
            *n++ = (float)(sin(theta)*sin(phi));
            *n++ = (float)(cos(theta));
            
        }
    }
    
    // build index array linking sets of three vertices into triangles
    std::vector<GLint>::iterator k = indices.begin();
    for (int i = 0; i<iFactor; i++) {
        for (int j = 0; j<iFactor; j++) {
            *k++ = i*iFactor + j;
            *k++ = (i+1)*iFactor + j;
            *k++ = i*iFactor + (j + 1);
            *k++ = (i+1)*iFactor + j;
            *k++ = (i+1)*iFactor + (j + 1);
            *k++ = i*iFactor + (j + 1);
            
        }
    }
    
    // load vertex and index array to GPU
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, iFactor*iFactor*sizeof(float[3]), &vert.front(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (iFactor*iFactor)*sizeof(int[6]), &indices.front(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, iFactor*iFactor*sizeof(float[3]), &normal.front(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    
    // initial shader load
    shaderParts[0].id = glCreateShader(GL_VERTEX_SHADER);
    shaderParts[0].file = "cubemapping.vert";
    shaderParts[1].id = glCreateShader(GL_FRAGMENT_SHADER);
    shaderParts[1].file = "cubemapping.frag";
    shaderID = glCreateProgram();
    updateShaders();

}

//
// Delete data
//
MObject::~MObject()
{
    glDeleteProgram(shaderID);
    glDeleteTextures(1, &textureID);
    glDeleteBuffers(NUM_BUFFERS, bufferIDs);
}


void MObject::createCubeTexture()
{
    //generate framebuffers
    glGenFramebuffersEXT(6, cube_framebuffer);
    
    //generate textures
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, CUBE_TEXTURE_SIZE, CUBE_TEXTURE_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, CUBE_TEXTURE_SIZE, CUBE_TEXTURE_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, CUBE_TEXTURE_SIZE, CUBE_TEXTURE_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, CUBE_TEXTURE_SIZE, CUBE_TEXTURE_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, CUBE_TEXTURE_SIZE, CUBE_TEXTURE_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, CUBE_TEXTURE_SIZE, CUBE_TEXTURE_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    
}

void MObject::updateShaders()
{
    loadShaders(shaderID, sizeof(shaderParts)/sizeof(*shaderParts),
                shaderParts);
    glUseProgram(shaderID);
    
    // (re)connect view and projection matrices
    glUniformBlockBinding(shaderID,
                          glGetUniformBlockIndex(shaderID,"Matrices"),
                          AppContext::MATRIX_UNIFORMS);
    
    // re-connect attribute arrays
    glBindVertexArray(varrayIDs[TERRAIN_VARRAY]);     //
    positionAttrib = glGetAttribLocation(shaderID, "vPosition");
    glEnableVertexAttribArray(positionAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    normalAttrib = glGetAttribLocation(shaderID, "vNormal");
    glEnableVertexAttribArray(normalAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glUniform1i(glGetUniformLocation(shaderID, "cubeTexture"), textureID);
    
    // turn off everything we enabled
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


void MObject::draw()
{
    updateShaders();
    // enable shaders
    glUseProgram(shaderID);
    
    //texture
    glActiveTexture(GL_TEXTURE0 + textureID);   //after terrain 4 textures
    glBindTexture(GL_TEXTURE_CUBE_MAP , textureID);
    
    // enable vertex arrays
    glBindVertexArray(varrayIDs[TERRAIN_VARRAY]);
    
    // draw the triangles for each three indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glDrawElements(GL_TRIANGLES, iFactor*iFactor*6, GL_UNSIGNED_INT, 0);
    
    // turn of whatever we turned on
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}