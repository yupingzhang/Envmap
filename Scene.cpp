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
    viewSph(vec3<float>(0.f, -1.4f, 500.f)),
    lightSph(vec3<float>(0.5f * F_PI, 0.25f * F_PI, 300.f))
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
    light();
}

//
// New view, pointing to origin, at specified angle
//
void Scene::view()
{
    // update view matrix
    sdata.viewmat = translate4fp(vec3<float>(0,0,-viewSph.z)) 
        * xrotate4fp(viewSph.y)
        * zrotate4fp(viewSph.x);
}

MatPair4f Scene::getView()
{
    return sdata.viewmat;
}

void Scene::setView(MatPair4f viewMatrix)
{
    sdata.viewmat = viewMatrix;
}

void Scene::setView(Vec3f rotate, Vec3f offset)
{
    sdata.viewmat = zrotate4fp(rotate.z)* yrotate4fp(rotate.y)* xrotate4fp(rotate.x)*translate4fp(offset);
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

//
// Call to update light position
//
void Scene::light()
{
    // update position from spherical coordinates
    float cx = cos(lightSph.x), sx = sin(lightSph.x);
    float cy = cos(lightSph.y), sy = sin(lightSph.y);
    sdata.lightpos = lightSph.z * vec3(cx*cy, sx*cy, sy);

    // update uniform block
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[MATRIX_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 
                    offsetof(ShaderData, lightpos), sizeof(sdata.lightpos),
                    &sdata.lightpos);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
