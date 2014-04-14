//
// Simple GL example
//

#include "AppContext.hpp"
#include "Input.hpp"
#include "Scene.hpp"
#include "Octahedron.hpp"
#include "MObject.hpp"
#include "Cuberoom.hpp"

//#include "Mat.inl"
#include "Vec.inl"
//#include "MatPair.inl"

// using core modern OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

///////
// Clean up any context data
AppContext::~AppContext()
{
    // if any are NULL, deleting a NULL pointer is OK
    delete scene;
    delete input;
    delete lightmarker;
    delete object;
    delete cube;
}

///////
// GLFW callbacks must use extern "C"
extern "C" {

    //
    // called for GLFW error
    //
    void winError(int error, const char *description)
    {
        fprintf(stderr, "GLFW error: %s\n", description);
    }

    //
    // called whenever the window size changes
    //
    void reshape(GLFWwindow *win, int width, int height)
    {
        AppContext *appctx = (AppContext*)glfwGetWindowUserPointer(win);

        appctx->scene->viewport(win);
        appctx->input->redraw = true;
    }

    //
    // called when mouse button is pressed
    //
    void mousePress(GLFWwindow *win, int button, int action, int mods)
    {
        AppContext *appctx = (AppContext*)glfwGetWindowUserPointer(win);

        appctx->input->mousePress(win, button, action);
    }

    //
    // called when mouse is moved
    //
    void mouseMove(GLFWwindow *win, double x, double y)
    {
        AppContext *appctx = (AppContext*)glfwGetWindowUserPointer(win);

        appctx->input->mouseMove(win, appctx->scene, x,y);
    }

    // 
    // called on any keypress
    //
    void keyPress(GLFWwindow *win, int key, int scancode, int action, int mods)
    {
        AppContext *appctx = (AppContext*)glfwGetWindowUserPointer(win);

        if (action == GLFW_PRESS)
            appctx->input->keyPress(win, key, appctx->terrain, appctx->object);
        else if (action == GLFW_RELEASE)
            appctx->input->keyRelease(win, key);
    }
}

// initialize GLFW - windows and interaction
GLFWwindow *initGLFW(AppContext *appctx)
{
    // set error callback before init
    glfwSetErrorCallback(winError);
    if (! glfwInit())
        return 0;

    // ask for a window with dimensions 843 x 480 (HD 480p)
    // using core OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    GLFWwindow *win = glfwCreateWindow(843, 480, "OpenGL Demo", 0, 0);
    if (! win) {
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(win);

    // use GLEW on windows to access modern OpenGL functions
    glewExperimental = true;
    glewInit();

	// store context pointer to access application data
    glfwSetWindowUserPointer(win, appctx);

    // set callback functions to be called by GLFW
    glfwSetFramebufferSizeCallback(win, reshape);
    glfwSetKeyCallback(win, keyPress);
    glfwSetMouseButtonCallback(win, mousePress);
    glfwSetCursorPosCallback(win, mouseMove);

    // set OpenGL state
    glEnable(GL_DEPTH_TEST);      // tell OpenGL to handle overlapping surfaces
    glEnable(GL_TEXTURE_CUBE_MAP);
    return win;
}

int main(int argc, char *argv[])
{
    // collected data about application for use in callbacks
    AppContext appctx;

    // set up GLUT and OpenGL
    GLFWwindow *win = initGLFW(&appctx);
    if (! win) return 1;

    // initialize context (after GLFW)
    appctx.scene = new Scene(win);
    appctx.input = new Input;
    appctx.lightmarker = new Octahedron();
    
    appctx.object = new MObject();
    appctx.cube = new Cuberoom();
    
    //add an object and apply cube mapping
    // loop until GLFW says it's time to quit
    while (!glfwWindowShouldClose(win)) {
        // check for continuous key updates to view
        appctx.input->keyUpdate(appctx.scene);

        if (appctx.input->redraw) {
            // we're handing the redraw now
            appctx.input->redraw = false;
            MatPair4f temp_matrix = appctx.scene->getView();  //current view matrix
            
            // clear old screen contents
            glClearColor(.5f, .7f, .9f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //render to texture
            appctx.object->createCubeTexture();
            appctx.scene->proj(1);
            Vec3f offset = vec3<float>(0.0, 100.0, -100.0);
            
            for (int i=0; i<6; i++)
            {
                glEnable(GL_DEPTH_TEST);
                // bind the one (render-to-texture) framebuffer for one face
                glBindFramebufferEXT (GL_DRAW_FRAMEBUFFER, appctx.object->cube_framebuffer[i]);
                
                glBindTexture(GL_TEXTURE_CUBE_MAP, appctx.object->textureID);
                // clear the framebuffer's colour and depth buffers
                glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                switch (i) {
                    case 0:  //POSITIVE_X
                    {
                        appctx.scene->setView(vec3<float>(M_PI/2, M_PI/2, -M_PI/2), offset);
                        //appctx.scene->setView(vec3<float>(-M_PI/2, M_PI/2, 0.0), offset);
                        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, appctx.object->textureID,0);
                        break;
                    }
                    case 1:  //NEGATIVE_X
                    {
                        appctx.scene->setView(vec3<float>(-M_PI/2, -M_PI/2, -M_PI/2), offset);
                        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, appctx.object->textureID,0);
                        break;
                    }
                    case 2:  //POSITIVE_Y
                    {
                        appctx.scene->setView(vec3<float>(M_PI/2, M_PI, M_PI), offset);
                        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, appctx.object->textureID,0);
                        break;
                    }
                    case 3:  //NEGATIVE_Y
                    {
                        appctx.scene->setView(vec3<float>(-M_PI/2, M_PI, -M_PI), offset);
                        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, appctx.object->textureID,0);
                        break;
                    }
                    case 4:  //POSITIVE_Z
                    {
                        appctx.scene->setView(vec3<float>(0.0, M_PI, M_PI ), offset);
                        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, appctx.object->textureID,0);
                        break;
                    }
                    case 5:  //NEGATIVE_Z
                    {
                        appctx.scene->setView(vec3<float>(M_PI, M_PI, 0.0), offset);
                        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, appctx.object->textureID,0);
                        break;
                    }
                }
                
                //does it work?
//                GLenum status;
//                status = glCheckFramebufferStatusEXT(GL_DRAW_FRAMEBUFFER);
//                printf("Framebuffer status: %d .\n", status);
  
                //draw the scene except the object
                appctx.scene->update();
                //add render the cube environment
                appctx.cube->draw();
                //appctx.terrain->draw();
                appctx.lightmarker->draw();
              
            }
            
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glUseProgram(0);
            
            //final
            //set the matrix back
            appctx.scene->setView(temp_matrix);
            appctx.scene->proj(0);
            // clear old screen contents
            //glClearColor(.5f, .7f, .9f, 1.f);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // bind default framebuffer
            glBindFramebuffer (GL_FRAMEBUFFER, 0);
          
            // draw something
            appctx.scene->update();
            //appctx.terrain->draw();
            appctx.lightmarker->draw();
            appctx.cube->draw();
            appctx.object->draw();

            // show what we drew
            glfwSwapBuffers(win);
        }

        // wait for user input
        glfwPollEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;
}
