/*----------------------------------------------*
 openFrameworks sketch for experimenting (err mostly debugging)
 feature detection and mesh building off of a video. 
 
 Toggle between using the thresholded tracked points
 to control jitter or the feature detection straight up
 on top.
 
 Make sure that the particles are turned off unless
 you want them, since they kill the framerate.
 
 Alex Wolfe @ the Studio for Creative Inquiry 
 *----------------------------------------------*/

#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxDelaunay.h"
#include "ofxXmlSettings.h"
#include "ofxGameCamera.h"
#include "ofxRGBDPlayer.h"
#include "ofxRGBDMeshBuilder.h"

#include "ofxUI.h"

#include "fmVert.h"
#include "fmVertController.h"

#include <iostream>

class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    //Core OF Utils
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
     RGBD Toolkit + Mesh Creation
     *----------------------------------*/
    ofxRGBDPlayer player;
    ofxRGBDMeshBuilder meshBuilder;
    ofxGameCamera cam;
    
    ofRectangle roi;
    ofFbo renderFBO;
    ofFbo previewFBO;
    
    ofMesh triangulatedMesh;
    
    ofRectangle meshRect;
    ofRectangle triangulatedRect;
    ofRectangle videoRect;
    
    bool renderMode;
    float farClip; //put into the GUI TODO
    
    void calculateRects();
    bool loadNewScene();
    bool loadDefaultScene();
    bool loadScene(string takeDirectory);
    int renderedVidoeFrame;
    
    /*----------------------------------*
     Feature Detection
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
    ofxDelaunay dTriangles;
    
    std::vector<cv::Point2f> harrisPoints;
    std::vector<ofPoint> featurePoints;
    
    void createTriangleMesh(float minDist);
    
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
    
    void guiSetup();
    void guiEvent(ofxUIEventArgs &e);
    
    float vidOffsetX, vidOffsetY;
    float labelOffset;
    
    
    
};
