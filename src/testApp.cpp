#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    img.loadImage("regine.png");
    bwImg.loadImage("regine.png");
    bwImg.setImageType(OF_IMAGE_GRAYSCALE);
    
    video.loadMovie("regineMov.mov");
    video.play();
    
    featureMax = 1500;
    featureQuality = 0.001;
    featureMinDist = 4;
    
    dTriangles.setMaxPoints(1504);
    triDraw = true;
}

//--------------------------------------------------------------
void testApp::update(){
    ofBackground(255);
    
    //update image to current frame
    img.clear();
    img = ofImage(video.getPixelsRef());
    bwImg.clear();
    bwImg = ofImage(img.getPixelsRef());
    bwImg.setImageType(OF_IMAGE_GRAYSCALE);
    
    //empty point vectors
    harrisPoints.clear();
    featurePoints.clear();
    
    //pull new feature points from the current frame
    featurePoints = featureDetect();
    
  
    //triangulate the found points using ofxTriangle
    triangle.clear();
    triangle.triangulate(featurePoints);
    
    //triangulate the found points using ofxDelauay
    dTriangles.reset();
    
    for(ofPoint p: featurePoints)
        dTriangles.addPoint(p);
    
    dTriangles.triangulate();
    
    
}

//--------------------------------------------------------------
void testApp::draw(){
    //  ofSetColor(0,255,255);
    //img.draw(0,0);
    video.draw(0,0);
    
    //draw the Triangles
    if(triDraw){
        for( int i=0; i<dTriangles.triangles.size(); i++ )
        {
            float centerX, centerY;
            unsigned char * imgPixels;
            imgPixels= img.getPixels();
            int colorIndex;
            // t.points[ 0 ].y * img.width + t.points[ 0 ].x]
            

                ofxDelaunayTriangle& tri = dTriangles.triangles[ i ];
                centerX = (tri.points[0].x + tri.points[1].x + tri.points[2].x)/3;
                centerY = (tri.points[0].y + tri.points[1].y + tri.points[2].y)/3;
                
                ofFill();
                ofColor c = img.getColor(centerX, centerY);
                c.setSaturation(c.getSaturation() + 50);
                ofSetColor(c);
                
                ofTriangle
                (
                 tri.points[ 0 ].x,
                 tri.points[ 0 ].y,
                 tri.points[ 1 ].x,
                 tri.points[ 1 ].y,
                 tri.points[ 2 ].x,
                 tri.points[ 2 ].y
                 );
        }
    }
	
    else{
        ofSetColor(255,0,0);
        ofNoFill();
        //triangle.draw();
        dTriangles.draw();
        
        ofSetColor(0, 255, 255);
        for(ofPoint p : featurePoints){
            ofEllipse(p.x, p.y, 2, 2);
        }
    }
    

    
    ofSetColor(255);
    
}


/*-------------------------------------------------*
 use OpenCv's feature detection to pull out interesting points
 *-------------------------------------------------*/
vector<ofPoint> testApp::featureDetect(){
    goodFeaturesToTrack(ofxCv::toCv(bwImg), harrisPoints, featureMax, (double)featureQuality, (double)featureMinDist);
    return ofxCv::toOf(harrisPoints).getVertices();
}



//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if(key == 't'){
        triDraw = !triDraw;
    }
    if( key == ' '){
        if(video.isPlaying())
            video.stop();
        else {
            video.play();
        }
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
    
}