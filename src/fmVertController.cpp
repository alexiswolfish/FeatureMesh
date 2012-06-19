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
    for(vector<fmVert>::iterator v1 = verts.begin(); v1 != verts.end(); ++v1){
        v1->pullToCenter(ofVec3f(w/2, h/2, 0));
        v1->update();
    }
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
 Pull points to the feature points
 *-----------------------------------*/

void fmVertController::pullToFeature(vector<ofPoint> features, float maxDist){
    //iterate through each point returned by the feature detection
    for(ofPoint f : features){
        //apply attraction to each particle relative to distance
        for(vector<fmVert>::iterator v = verts.begin(); v != verts.end(); ++v){
            ofVec3f dir = v->loc - f;
            float dist = dir.length();
            
            //only apply forces within a certain radius?
            //might want to just say if dist >0
            //also has to be less strong than the repulsion force
            if(dist < maxDist){
                dir.normalize();
                v->vel -= dir * (dist) * (1/(dist*dist));
            }
        }
    }
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
            
           // if(distSq < repulsion){
            if(distSq > 0){  
                float F = 1.0/distSq *repulsion;
               // float F = (repulsion/distSq - 1.0)* 0.01;
                dir.normalize();
              //  dir = dir.normalize()*F;
                v1->acc += dir *(F/v1->mass);
                v2->acc -= dir *(F/v2->mass);
            }
        }
        
    }
}

void fmVertController::setNumVerticies(int num, float maxSpeed){
    if(num == verts.size()){}
    else if(num < verts.size())
        removeVerticies(verts.size()-num);
    else if(num > verts.size())
        addVerticies(num - verts.size(), maxSpeed);
}

void fmVertController::addVerticies(int num, float _maxSpeed){
    
    float space = w*h/num;
    for(int i=0; i<num; i++){
        fmVert v = fmVert(ofVec3f(ofRandom(w),ofRandom(h),0), _maxSpeed, i);
        verts.push_back(v);
    }
}

void fmVertController::addVerticies(int num, float _maxSpeed, ofVec3f initPos){
    for(int i=0; i<num; i++){
        fmVert v = fmVert(initPos, _maxSpeed, verts.size()+1);
        verts.push_back(v);
    }
}

void fmVertController::removeVerticies(int num){
    for(int i=0; i<num; i++)
        verts.pop_back();
}

//repulsion should be squared?

//robert hodgin's particle code from the hello cinder tutorial
void fmVertController::attract(float repulsion, float thresh){
    for(vector<fmVert>::iterator v1 = verts.begin(); v1 != verts.end(); ++v1){
        vector<fmVert>::iterator v2 = v1;
        for(++v2; v2 != verts.end(); ++v2){
            ofVec3f dir = v1->loc - v2->loc;
            float distSq = dir.lengthSquared();
            
            if(distSq < repulsion){
                float percent = distSq/repulsion;
                
                if(percent < thresh){
                    float F = ( thresh/percent - 1.0f ) * 0.01f;
					dir.normalize();
					dir *= F;
                    
					v1->acc += dir;
					v2->acc -= dir;
                }
                else {						// Cohesion
					float threshDelta = 1.0f - thresh;
					float adjustedPercent = ( percent - thresh )/threshDelta;
					//float F = ( 1.0 - ( cos( adjustedPercent * pi*2 ) * -0.5f + 0.5f ) ) * 0.05f;
					
					// INTERESTING BUG
					// Use this F instead and lower the thresh to 0.2 after flattening the scene ('f' key)
                    float F = ( 0.5f - ( cos( adjustedPercent * pi*2 ) * 0.5f + 0.5f ) ) * 0.15f;
                    
					dir.normalize();
					dir *= F;
                    
					v1->acc -= dir;
					v2->acc += dir;
					
				}
            }
        }
    }
}
