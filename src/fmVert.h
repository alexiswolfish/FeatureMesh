/*---------------------
 Alex Wolfe @ the studio for creative inquiry
 *---------------------*/

#ifndef FeatureMesh_fmVert_h
#define FeatureMesh_fmVert_h

#include "ofMain.h"

class fmVert{
    
public:
    
    fmVert(ofVec3f initLoc);
    
    //updates the Particle's position in space
    void update();
    void draw();
    
    //basic physics variables
    ofVec3f loc;
    ofVec3f vel;
    ofVec3f acc;
    
    float radius;
};

#endif
