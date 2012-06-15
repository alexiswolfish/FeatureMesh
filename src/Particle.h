/*--------------------------------------------------------------*
   Particle.h
 
 A quick Particle class to use in conjunction with feature detection
 for James George's RGDB Visualize project.
 
 Alex Wolfe @ the studio for creative inquiry
*---------------------------------------------------------------*/

#ifndef delaunayFeatureMesh_Particle_h
#define delaunayFeatureMesh_Particle_h

#include "ofMain.h"

class Particle {
    
public:
    
    Particle();
    
    ofPoint pos;
    ofPoint vel;
    ofPoint acc;
}

#endif
