// data shared across entire scene
// expected to change up to once per frame
// primarily view information

#include "Scene.hpp"
#include "MatPair.inl"
#include "Vec.inl"
#include "AppContext.hpp"

// using core modern OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// for offsetof
#include <cstddef>

#ifndef F_PI
#define F_PI 3.1415926f
#endif

//
// create and initialize view
//
Scene::Scene(GLFWwindow *win) : 
    viewSph(vec3<float>(0.f, -1.4f, 500.f))
{
    // create uniform buffer objects
    glGenBuffers(NUM_BUFFERS, bufferIDs);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[MATRIX_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderData), 0, GL_STREAM_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, AppContext::MATRIX_UNIFORMS,
                     bufferIDs[MATRIX_BUFFER]);

    // update view
    viewport(win);
    view();
}

//
// New view, pointing to origin, at specified angle
//
void Scene::view()
{
    // update view matrix
    sdata.viewmat = translate4fp(vec3<float>(0,0,-viewSph.z))
        * xrotate4fp(viewSph.y)
        * yrotate4fp(viewSph.x);
}

Vec3f Scene::getviewDirection()
{
    float x = sin(viewSph.x)*cos(viewSph.y)*viewSph.z;
    float z = cos(viewSph.x)*cos(viewSph.y)*viewSph.z;
    float y = sin(viewSph.y)*viewSph.z;
    
    return normalize(vec3<float>(x,y,z));
}

MatPair4f Scene::getView()
{
    return sdata.viewmat;
}

void Scene::setView(MatPair4f viewMatrix)
{
    sdata.viewmat = viewMatrix;
}

void Scene::setView(Vec3f rotate, Vec3f position)
{
    sdata.viewmat = zrotate4fp(rotate.z)* yrotate4fp(rotate.y)* xrotate4fp(rotate.x)*translate4fp(position);
}

void Scene::proj(int cubeflag)
{
    if (cubeflag == 1)   //compute the cube box
    {
        sdata.projection = perspective4fp(F_PI/2, 1, 50, 10000);
    }
    else   //normal
        sdata.projection = perspective4fp(F_PI/4, (float)width/height, 1, 10000);
    
}

//
// This is called when window is created or resized
// Adjust projection accordingly.
//
void Scene::viewport(GLFWwindow *win)
{
    // get window dimensions
    glfwGetFramebufferSize(win, &width, &height);

    // this viewport makes a 1 to 1 mapping of physical pixels to GL
    // "logical" pixels
    glViewport(0, 0, width, height);

    // adjust 3D projection into this window
    sdata.projection = perspective4fp(F_PI/4, (float)width/height, 1, 10000);
}

//
// call before drawing each frame to update per-frame scene state
//
void Scene::update() const
{
    // update uniform block
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[MATRIX_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ShaderData), &sdata);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::objectmove()
{
    sdata.objectpos = objPos;
    
    // update uniform block
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[MATRIX_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER,
                    offsetof(ShaderData, objectpos), sizeof(sdata.objectpos),
                    &sdata.objectpos);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}





