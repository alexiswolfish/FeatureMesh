//
//  fmVert.cpp
//  FeatureMesh
//
//  Created by Golan Levin on 6/15/12.
//  Copyright (c) 2012 Flong. All rights reserved.
//

#include <iostream>
#include "fmVert.h"

fmVert::fmVert(ofVec3f initLoc){
    loc = initLoc;
    vel = ofVec3f(0,0,0);
    acc = ofVec3f(0,0,0);
    radius = 0.5;
    
}

void fmVert::update(){
    
    loc += vel*acc;
    
}

void fmVert::draw(){
    ofPushStyle();
    ofSetColor(0,255,255);
    ofEllipse(loc.x, loc.y, radius, radius);
    ofPopStyle();
    
}