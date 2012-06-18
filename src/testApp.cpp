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
    
    dTriangles.setMaxPoints(featureMax);
    
    featureDraw = false;
    triDraw = false;
    particleDraw = true;
    
    /*--------Particles-----------*/
    particles = fmVertController(video.width, video.height);
    particles.setNumVerticies(featureMax);
    rep =3;
    
    /*--------GUI-----------*/
    vidOffsetX = 900 - video.width;
    vidOffsetY = (600 - video.height)/2;
    labelOffset = 20;
    float dim = 16;
    
  //  setFont(OFX_UI_FONT_NAME,true, true, false, 0.0, OFX_UI_FONT_RESOLUTION);
    
    gui = new ofxUICanvas(0,0,vidOffsetX, ofGetHeight());
    
    gui->addWidgetDown(new ofxUILabel("FEATURE DETECTION", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(new ofxUISpacer(vidOffsetX - labelOffset, 2)); 
    gui->addWidgetDown(new ofxUILabelToggle(vidOffsetX-labelOffset, false, "draw feature mesh", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUILabelToggle(vidOffsetX-labelOffset, false, "draw triangulation", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIRotarySlider(dim*4, 50, 2000, featureMax, "max number of features")); 
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 0.001, 0.02, featureQuality, "feature quality")); 
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 1, 30, featureMinDist, "feature distance"));
    gui->addWidgetDown(new ofxUISpacer(vidOffsetX - labelOffset, 2)); 
    gui->addWidgetDown(new ofxUILabel("PARTICLE CONTROL", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(new ofxUISpacer(vidOffsetX - labelOffset, 2)); 
    gui->addWidgetDown(new ofxUILabelToggle(vidOffsetX-labelOffset, true, "draw particle mesh", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 50, 5000, featureMax, "number of particles")); 
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 1, 5, rep, "repulsion")); 
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    /*-------------------*/
    
    ofEnableSmoothing(); 
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
    dTriangles.setMaxPoints(featureMax);
    
    for(ofPoint p: featurePoints)
        dTriangles.addPoint(p);
    
    dTriangles.triangulate();
    
    /*------------update particles-------------*/
    particles.separate(rep);
    particles.update();
    
    
}

//--------------------------------------------------------------
void testApp::draw(){
    img.draw(vidOffsetX, vidOffsetY);
    
    ofPushMatrix();
    ofTranslate(vidOffsetX, vidOffsetY);
    
    //draw the Triangles
    if(featureDraw){
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
	
    else if(triDraw){
        ofSetColor(255,0,0);
        ofNoFill();
        //triangle.draw();
        dTriangles.draw();
        
        ofSetColor(0, 255, 255);
        for(ofPoint p : featurePoints){
            ofEllipse(p.x, p.y, 2, 2);
        }
    }
    
    else if(particleDraw){
        particles.draw();
    }
    
    ofPopMatrix();
    

    
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
void testApp::exit()
{
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui; 
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
    
    if(name == "draw feature mesh"){
        ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
        featureDraw = toggle->getValue();
        particleDraw = false;
    }
    if(name == "draw triangulation"){
        ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
        triDraw = toggle->getValue();
        particleDraw = false;
    }
    else if(name == "draw particle mesh"){
        ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
        particleDraw = toggle->getValue();
        featureDraw = false;
    }
    else if(name == "max number of features"){
        ofxUIRotarySlider *slider = (ofxUIRotarySlider *) e.widget;
        featureMax = slider->getScaledValue();
    }
    else if(name == "feature quality")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		featureQuality = slider->getScaledValue(); 
	}
    else if(name == "feature distance")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		featureMinDist = slider->getScaledValue(); 
	}
    else if(name == "number of particles")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        particles.setNumVerticies(slider->getScaledValue());
	}
    else if(name == "repulsion")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        rep = (slider->getScaledValue());
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
    particles.addVerticies(1, ofVec3f(mouseX, mouseY, 0));
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