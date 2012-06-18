/*---------------------
 Alex Wolfe @ the studio for creative inquiry
 *---------------------*/

#include <iostream>
#include "fmVertController.h"

fmVertController::fmVertController(int width, int height){
    w = width;
    h = height;
}

fmVertController::fmVertController(){
    //default low res video values
    w = 640;
    h = 380;
}
void fmVertController::update(){
    for(vector<fmVert>::iterator v1 = verts.begin(); v1 != verts.end(); ++v1)
        v1->update();
}

void fmVertController::draw(){
    for(fmVert v: verts)
        v.draw();
}

void fmVertController::draw(ofImage img){
    for(fmVert v: verts)
        v.draw(img.getColor(v.loc.x, v.loc.y));
        
}

/*-----------------------------------*
    Repel particles from one another
 
 Force is inverse of distSq betwen particles
 F = m*A
 *-----------------------------------*/
void fmVertController::separate(float repulsion){
    for(vector<fmVert>::iterator v1 = verts.begin(); v1 != verts.end(); ++v1){
        vector<fmVert>::iterator v2 = v1;
        for(++v2; v2 != verts.end(); ++v2){
            
            ofVec3f dir = v1->loc - v2->loc;
            float distSq = dir.lengthSquared();
            
            if(distSq > 0.0){
                dir.normalize();
                float F = 1.0/distSq/repulsion;
                
                v1->acc += dir *(F/v1->mass);
                v2->acc -= dir *(F/v2->mass);
            }
        }
        
    }
}

void fmVertController::setNumVerticies(int num){
    if(num == verts.size()){}
    else if(num < verts.size())
        removeVerticies(verts.size()-num);
    else if(num > verts.size())
        addVerticies(num - verts.size());
}

void fmVertController::addVerticies(int num){
    
    float space = w*h/num;
    for(int i=0; i<num; i++){
        fmVert v = fmVert(ofVec3f(ofRandom(w),ofRandom(h),0), i);
        verts.push_back(v);
    }
}

void fmVertController::addVerticies(int num, ofVec3f initPos){
    for(int i=0; i<num; i++){
        fmVert v = fmVert(initPos, verts.size()+1);
        verts.push_back(v);
    }
}

void fmVertController::removeVerticies(int num){
    for(int i=0; i<num; i++)
        verts.pop_back();
}
