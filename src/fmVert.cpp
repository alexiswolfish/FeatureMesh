/*---------------------
 Alex Wolfe @ the studio for creative inquiry
 *---------------------*/

#include <iostream>
#include "fmVert.h"

fmVert::fmVert(ofVec3f initLoc, float _id){
    loc = initLoc;
    pLoc = initLoc;
    vel = ofVec3f(ofRandom(0,1), ofRandom(0,1), 0);
    acc = ofVec3f(0,0,0);
    decay = 0.95;
    radius = 2;
    mass = radius*radius*0.005;
    
    id = _id;
}

void fmVert::update(){
    pLoc.set(loc);
    vel = vel + acc;
    loc.set(loc + vel);
    vel.set(vel * decay);
    acc.set(0,0,0);
}

void fmVert::draw(){
    ofPushStyle();
    ofSetColor(255,0,255);
   // ofEllipse(loc.x, loc.y, radius, radius);
    ofLine(loc.x, loc.y, pLoc.x, pLoc.y);
    if(ofDist(loc.x, loc.y, pLoc.x, pLoc.y) == 0)
        ofEllipse(loc.x, loc.y, radius, radius);
    
    ofPopStyle();
    
}

void fmVert::draw(ofColor color){
    ofPushStyle();
    ofSetColor(color);
    //radius = (img.getColor(loc.x, loc.y)).getBrightness();
    ofEllipse(loc.x, loc.y, radius, radius);
    ofPopStyle();
}