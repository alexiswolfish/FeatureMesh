/*---------------------
 Alex Wolfe @ the studio for creative inquiry
 *---------------------*/

#ifndef FeatureMesh_fmVert_h
#define FeatureMesh_fmVert_h

#include "ofMain.h"

class fmVert{
    
public:
    
    fmVert(ofVec3f initLoc, float _maxSpeed, float _id);
    
    //updates the Particle's position in space
    void update();
    void pullToCenter(ofVec3f center);
    
    void draw();
    void draw(ofColor color);
    
    //basic physics variables
    ofVec3f pLoc;
    ofVec3f loc;
    ofVec3f vel;
    ofVec3f acc;
    
    float decay;
    float mass;
    float radius;
    
    float maxSpeed;
    float maxSpeedSq;
    
    float id;
};

#endif
