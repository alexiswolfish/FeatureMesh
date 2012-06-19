/*---------------------
 Alex Wolfe @ the studio for creative inquiry
 *---------------------*/

#ifndef FeatureMesh_fmVertController_h
#define FeatureMesh_fmVertController_h


#include "fmVert.h"
#include "ofMain.h"

class fmVertController{
public:
    
    fmVertController(int width, int height);
    fmVertController();
    void update();
    void draw();
    void draw(ofImage img);
    void addVerticies(int num, float maxSpeed);
    void addVerticies(int num, float maxSpeed, ofVec3f initPos);
    void removeVerticies(int num);
    void setNumVerticies(int num, float maxSpeed);
    
    void separate(float repulsion);
    void attract(float repulsion, float attract);
    
    void pullToFeature(std::vector<ofPoint> features, float maxDist);

    std::vector<fmVert> verts;
    
    int w,h;
};
#endif
