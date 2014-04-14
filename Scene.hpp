// data shared across entire scene
// expected to change up to once per frame
#ifndef Scene_hpp
#define Scene_hpp

#include "Vec.hpp"
#include "MatPair.hpp"

struct GLFWwindow;

class Scene {
// private data
private:
    struct ShaderData {
        MatPair4f viewmat, projection; // viewing matrices
        Vec3f objectpos;
    } sdata;

    // GL uniform buffer IDs
    enum {MATRIX_BUFFER, NUM_BUFFERS};
    unsigned int bufferIDs[NUM_BUFFERS];

// public data
public:
    
    int width, height;         // current window dimensions

    Vec3f viewSph;          // view position in spherical coordinates
    Vec3f objPos;
    
    // public methods
public:
    // create with initial window size and orbit location
    Scene(GLFWwindow *win);

    // set up new window viewport and projection
    void viewport(GLFWwindow *win);

    // set view using orbitAngle
    void view();

    Vec3f getEyePosition();
    
    //get
    MatPair4f getView();
    //set
    void setView(MatPair4f viewMatrix);
    void setView(Vec3f rotate, Vec3f pos);
    
    //update projection matrix
    void proj(int flag);
    
    //update object position
    void objectmove();
    
    // update shader uniform state each frame
    void update() const;
};

#endif
