

#include <iostream>
#include "fmVertController.h"

void fmVertController::update(){
    for(fmVert v: verts)
        v.update();
}

void fmVertController::draw(){
    for(fmVert v: verts)
        v.draw();
}

void fmVertController::addVerticies(int num){
    for(int i=0; i<num; i++){
        
    }
}

void fmVertController::removeVerticies(int num){
    
}
