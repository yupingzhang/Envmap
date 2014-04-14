// handle changes due to mouse motion, or keys
#ifndef Input_hpp
#define Input_hpp

class Scene;
class Terrain;
class MObject;
class Octahedron;
struct GLFWwindow;

class Input {
// private data
private:
    int button, oldButton;      // which mouse button was pressed?
    double oldX, oldY;          // location of mouse at last event

    double updateTime;          // time (in seconds) of last update
    float panRate, tiltRate;    // for key change, orbiting rate in radians/sec

// public data
public:
    bool redraw;                // true if we need to redraw
    bool redrawcubemap;         //only if light changes

// public methods
public:
    // initialize
    Input() : button(-1), oldButton(-1), oldX(0), oldY(0), 
              panRate(0), tiltRate(0), redraw(true), redrawcubemap(true) {}

    // handle mouse press / release
    void mousePress(GLFWwindow *win, int button, int action);

    // handle mouse motion
    void mouseMove(GLFWwindow *win, Scene *scene, double x, double y);

    // handle key press
    void keyPress(GLFWwindow *win, int key, Terrain *, MObject *);

    // handle key release
    void keyRelease(GLFWwindow *win, int key);

    // update view (if necessary) based on key input
    void keyUpdate(Scene *scene);
};

#endif
