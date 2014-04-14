//reflexible object
#ifndef MObject_hpp
#define MObject_hpp

#include <vector>

#include "Vec.hpp"
#include "Shader.hpp"

// using core modern OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define CUBE_TEXTURE_SIZE 512

class MObject {
private:
    
    Vec3f iPosition;

    int iFactor;
    int iRadius;
    unsigned int numvert;       // total vertices
    //Vec3f vert[4];              // per-vertex position
    std::vector<GLfloat> vert;
    
    unsigned int numtri;             // total triangles
    //Vec<unsigned int, 3> indices[4]; // 3 vertex indices per triangle
    std::vector<GLint> indices;
    
    unsigned int numnor;            //total normal
    //Vec3f normal[4];
    std::vector<GLfloat> normal;
    
    // GL vertex array object IDs
    enum {TERRAIN_VARRAY, NUM_VARRAYS};
    unsigned int varrayIDs[NUM_VARRAYS];
    
    // GL buffer object IDs
    enum {POSITION_BUFFER, INDEX_BUFFER, NORMAL_BUFFER, NUM_BUFFERS};  //ADD NORMAL
    unsigned int bufferIDs[NUM_BUFFERS];
    
    // GL shaders
    unsigned int shaderID;      // ID for shader program
    ShaderInfo shaderParts[2];  // vertex & fragment shader info
    
    // vertex attribute IDs
    unsigned int positionAttrib;
    unsigned int normalAttrib;
    
    //////////////////////////////////
    /////////////cubemap//////////////
public:
    //textures
    unsigned int textureID, depthbuffer;
    
    //framebuffer
    unsigned int cube_framebuffer[6];
    
public:
    MObject();
    ~MObject();

    void createCubeTexture();
    //void drawToFace(int face);
    // load/reload shaders
    void updateShaders();
    // draw this tetrahedron object
    void draw();

};

#endif
