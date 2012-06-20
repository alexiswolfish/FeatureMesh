#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxTriangle.h"
#include "ofxDelaunay.h"

#include "ofxUI.h"

#include "fmVert.h"
#include "fmVertController.h"

#include <iostream>

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
        void exit();
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    /*----------------------------------*
     Alex's feature detection 
     and mesh generation functions and vars
     *----------------------------------*/
    vector<ofPoint> featureDetect();
    ofColor colorSample(int x, int  y);
    
    int featureMax;
    float featureQuality;
    int featureMinDist;
    
    bool featureDraw;
    bool triDraw;
    bool particleDraw;
    bool trackerDraw;
    
    bool useTrackedPoints;
        
    ofImage img;
    ofImage bwImg;
    
    ofVideoPlayer video;
    
    ofxCv::ContourFinder contourFinder;
    ofxTriangle triangle;
    ofxDelaunay dTriangles;
    
    std::vector<cv::Point2f> harrisPoints;
    std::vector<ofPoint> featurePoints;
    
    /*----------------------------------*
     Tracker
     *----------------------------------*/
    ofxCv::PointTracker tracker;
    std::vector<unsigned int> curTrackedPoints;
    std::vector<unsigned int> prevTrackedPoints;
    
    int trackerMaxDist;
    int persistance;
    int age;
   // int maxTrackedPointDist;
    std::vector<cv::Point2f> trackedPoints;
    
    /*----------------------------------*
     Particle Variables
     *----------------------------------*/
    fmVertController particles;
    float rep;
    float maxSpeed;
    float maxDist;
    
    /*----------------------------------*
     Gui Features
     *----------------------------------*/
    
    ofxUICanvas *gui;
    ofxUICanvas *gui2;
    
    ofxUIMovingGraph *fpSize; 
    ofxUIMovingGraph *tpSize; 
    
    void guiEvent(ofxUIEventArgs &e);
    
    float vidOffsetX, vidOffsetY;
    float labelOffset;
    
    
    
};
