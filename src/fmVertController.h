

#ifndef FeatureMesh_fmVertController_h
#define FeatureMesh_fmVertController_h


#include "fmVert.h"
#include "ofMain.h"

class fmVertController{
public:
    
    fmVertController();
    void update();
    void draw();
    void addVerticies(int num);
    void removeVerticies(int num);

    std::vector<fmVert> verts;
}
#endif
