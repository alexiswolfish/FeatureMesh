/*---------------------
 Alex Wolfe @ the studio for creative inquiry
 *---------------------*/

#include <iostream>
#include "fmVert.h"

fmVert::fmVert(ofVec3f initLoc, float _maxSpeed, float _id){
    loc = initLoc;
    pLoc = initLoc;
    //vel = ofVec3f(ofRandom(0,1), ofRandom(0,1), 0);
    vel = ofVec3f(0,0,0);
    acc = ofVec3f(0,0,0);
    decay = 0.95;
    radius = 2;
    mass = radius*radius*0.005;
    
    maxSpeed = _maxSpeed;
    maxSpeedSq = maxSpeed*maxSpeed;
    
    id = _id;
}

void fmVert::update(){
    pLoc.set(loc);
    vel = vel + acc;
    
    //limit velocity 
    if(vel.lengthSquared() > maxSpeedSq){
        vel.normalize();
        vel *= maxSpeed;
    }
    
    loc.set(loc + vel);
    vel.set(vel * decay);
    acc.set(0,0,0);
}

void fmVert::pullToCenter(ofVec3f center){
    ofVec3f dirToCenter = loc - center;
    float distToCenter = dirToCenter.length();
    float maxDist = 100; //make this adjustable (img.height/2)
    
    if(distToCenter > maxDist){
        dirToCenter.normalize();
        float pullStrength = 0.01; //also make this adjustable
        vel -= dirToCenter * ((distToCenter - maxDist)*pullStrength);
    }
    
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