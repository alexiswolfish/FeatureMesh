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
    void addVerticies(int num);
    void addVerticies(int num, ofVec3f initPos);
    void removeVerticies(int num);
    void setNumVerticies(int num);
    
    void separate(float repulsion);

    std::vector<fmVert> verts;
    
    int w,h;
};
#endif
