#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    //load your video file here. This bit is antiquated for testing
    img.loadImage("regine.png");
    bwImg.loadImage("regine.png");
    bwImg.setImageType(OF_IMAGE_GRAYSCALE);
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    /*--------Game Camera------*/
    cam.setup();
    cam.speed = 20;
    cam.autosavePosition = true;
    cam.targetNode.setPosition(ofVec3f());
    cam.targetNode.setOrientation(ofQuaternion());
    cam.targetXRot = -180;
    cam.targetYRot = 0;
    cam.rotationZ = 0;    
    cam.setScale(1,-1,1);
    
    vidOffsetX = 250;
    vidOffsetY = 100;
    labelOffset = 20;
    
    //random stuff from James' code that I need to figure out what they do
    renderMode = false;
    farClip = 1245;
    
    calculateRects();
    loadDefaultScene();
    //loadNewScene();
    
    /*--------Feature Detection-----------*/
    featureMax = 1500;
    featureQuality = 0.001;
    featureMinDist = 4;
    
    featureDraw = false;
    triDraw = false;
    particleDraw = false;
    trackerDraw = false;
    useTrackedPoints = true;
    
    /*--------Particles-----------*/
    rep =.2;
    maxSpeed = 3;
    maxDist = 50;
    
    // particles = fmVertController(player.getVideoPlayer().width, player.getVideoPlayer().height);
    particles = fmVertController(640,380);
    particles.setNumVerticies(800, maxSpeed);
    
    /*--------Tracker-----------*/
    
    trackerMaxDist = 64;
    persistance = 15;
    age = 5;
    
    /*--------GUI-----------*/
    
    guiSetup();
    
    /*-------------------*/
    
    
    ofEnableSmoothing(); 
}

//--------------------------------------------------------------
bool testApp::loadNewScene(){
    ofFileDialogResult r = ofSystemLoadDialog("Select a Scene", true);
    if(r.bSuccess){
        return loadScene(r.getPath());
    }
    return false;
}

bool testApp::loadDefaultScene(){
    ofxXmlSettings settings;
    if(settings.loadFile("RGBDSimpleSceneDefaults.xml")){
        if(!loadScene(settings.getValue("defaultScene", ""))){
            return loadNewScene();
        }
        return true;
    }
    return loadNewScene();
}

bool testApp::loadScene(string takeDirectory){
    //use the low res, high kills my machine
    if(player.setup(takeDirectory, false)){
        ofxXmlSettings settings;
        settings.loadFile("RGBDSimpleSceneDefaults.xml");
        settings.setValue("defaultScene", player.getScene().mediaFolder);
        settings.saveFile();
        
        meshBuilder.setup(player.getScene().calibrationFolder);
        
        //populate
        player.getVideoPlayer().setPosition(.5);
        player.update();
        
        meshBuilder.setXYShift(player.getXYShift());
        //this will compensate if we are using an offline video that is of a different scale
        //meshBuilder.setTextureScaleForImage(player.getVideoPlayer()); 
        //update the first mesh
        meshBuilder.updateMesh(player.getDepthPixels());
        
        return true;
    }
    return false;
}

//--------------------------------------------------------------
void testApp::update(){
    
    ofBackground(180);
    
    //JG bleeeh
    //Rects + Camera update
    cam.applyRotation = cam.applyTranslation = meshRect.inside(mouseX, mouseY) || triangulatedRect.inside(mouseX, mouseY);
    //put stuff if you want to screw with Jon's alignment here (TODO)
    
    /*---------------Update player----------------*/
    
    player.update();
    if(player.isFrameNew())
        meshBuilder.updateMesh(player.getDepthPixels());
    
    player.getVideoPlayer().setFrame( player.getVideoPlayer().getCurrentFrame() + 1);
    player.update(); //Do I really want to call update twice here? TODO
    
    
    //update image to current frame
    img.clear();
    //img.setUseTexture(false);
    img.setFromPixels(player.getVideoPlayer().getPixelsRef());
    
    bwImg.clear();
    bwImg = ofImage(img.getPixelsRef());
    bwImg.setImageType(OF_IMAGE_GRAYSCALE);
    
    //empty point vectors
    harrisPoints.clear();
    featurePoints.clear();
    
    //pull new feature points from the current frame
    featurePoints = featureDetect();
    
    /*------------Tracking-------------*/
    tracker.setPersistence(persistance);
    tracker.setMaximumDistance(trackerMaxDist);
    
    tracker.track(ofxCv::toCv(featurePoints));
    
    //create a new vector only comprised of points that haven't moved
    //too much between frames
    trackedPoints.clear();
    
    for(int curIndex : tracker.getCurrentLabels()){
        if(tracker.existsPrevious(curIndex)){
            ofxCv::Point2f& prev = tracker.getPrevious(curIndex);
            ofxCv::Point2f& cur = tracker.getCurrent(curIndex);
            
            ofVec2f dir = ofxCv::toOf(prev) - ofxCv::toOf(cur);
            float dist = dir.length();
            
            trackedPoints.push_back(cur);
        }
    }
    /*---------------Triangulate------------------*/
    
    //triangulate the found points using ofxDelaunay
    if( featureDraw || triDraw || trackerDraw){
        dTriangles.reset();
        if(useTrackedPoints){
            for(cv::Point2f p : trackedPoints)
                dTriangles.addPoint(ofxCv::toOf(p));
        }
        else{
            for(ofPoint p: featurePoints)
                dTriangles.addPoint(p);
        }
        dTriangles.triangulate();
        createTriangleMesh(5.0);
    }
    
    /*------------update particles-------------*/
    
    if(particleDraw){
        particles.separate(rep);
        //particles.attract(rep*rep, 0.65);
        particles.pullToFeature(trackedPoints, maxDist);
        particles.update();
    }
    
    /*------------update GUI-------------*/
    
    fpSize->addPoint(featurePoints.size());
    tpSize->addPoint(trackedPoints.size());
}

//--------------------------------------------------------------
void testApp::draw(){
    if(player.isLoaded()){
        player.getVideoPlayer().draw(videoRect);
        
        //...with feature overlay
        if(triDraw){
            ofPushMatrix();
            ofTranslate(videoRect.x, videoRect.y);
            ofScale(videoRect.width / player.getVideoPlayer().getWidth(), 
                    videoRect.height / player.getVideoPlayer().getHeight());
            ofPushStyle();
            ofNoFill();
            ofSetColor(255, 0, 0);
            ofSetLineWidth(0.5);
            dTriangles.triangleMesh.drawWireframe();
            ofPopStyle();
            ofPopMatrix();
        }
        
        renderFBO.begin();
        ofClear(0,0,0,0);
        //draw triangulated mesh
        cam.begin();
        ofPushMatrix();
        ofPushStyle();
        ofScale(1,-1, 1);
        glEnable(GL_DEPTH_TEST);
        player.getVideoPlayer().getTextureReference().bind();
        ofEnableAlphaBlending();
        ofSetColor(255, 255, 255, 100);
        triangulatedMesh.draw();
        ofSetLineWidth(4);
        ofBlendMode(OF_BLENDMODE_ADD);
        triangulatedMesh.drawWireframe();
        player.getVideoPlayer().getTextureReference().unbind();
        glDisable(GL_DEPTH_TEST);
        ofPopStyle();
        ofPopMatrix();
        cam.end();
        renderFBO.end();
        
        renderFBO.getTextureReference().draw(triangulatedRect);
    }
    
    //draw 2D triangulization abstraction on the image
    if(featureDraw){
        ofPushMatrix();
        ofTranslate(videoRect.x, videoRect.y);
        ofScale(videoRect.width / player.getVideoPlayer().getWidth(), 
                videoRect.height / player.getVideoPlayer().getHeight());
        ofPushStyle();

        //argh getIndicies might be hella slow, you really want the pointer.
        for( int i=0; i<dTriangles.triangleMesh.getIndices().size(); i+=3 )
        {
            float centerX, centerY;
            unsigned char * imgPixels;
            imgPixels= img.getPixels();
            int colorIndex;
            // t.points[ 0 ].y * img.width + t.points[ 0 ].x]
            
            //oh god, this may be absolutely catestrophic
            //should find a cleaner way to do this
            ofVec3f a = dTriangles.triangleMesh.getVerticesPointer()[*(dTriangles.triangleMesh.getIndexPointer()+i)];
            ofVec3f b = dTriangles.triangleMesh.getVerticesPointer()[*(dTriangles.triangleMesh.getIndexPointer()+i+1)];
            ofVec3f c = dTriangles.triangleMesh.getVerticesPointer()[*(dTriangles.triangleMesh.getIndexPointer()+i+2)];
            
            centerX = (a.x + b.x + c.x)/3;
            centerY = (a.y + b.y + c.y)/3;
            
            ofFill();
            ofColor triColor = colorSample(centerX, centerY);
            //eh don't do this until its on a slider, probably not worth it
            //triColor.setSaturation(triColor.getSaturation() + 10);
            
            ofSetColor(triColor);
            ofTriangle(a.x,a.y,b.x,b.y,c.x,c.y);
        }
        
        ofPopStyle();
        ofPopMatrix();
    }
    if(trackerDraw){
        ofSetLineWidth(1);
        if(!useTrackedPoints){
            for(ofPoint p : featurePoints)
                ofEllipse(p.x, p.y, 2, 2);
        }
        for(int curIndex : tracker.getCurrentLabels()){
            if(tracker.existsPrevious(curIndex)){
                cv::Point2f& prev = tracker.getPrevious(curIndex);
                cv::Point2f& cur = tracker.getCurrent(curIndex);
                
                ofSetColor(255,0, (curIndex%255));
                ofLine(prev.x, prev.y, cur.x, cur.y);
            }
        }
    }
    else if(particleDraw){
        particles.draw();
    }
    
    ofPopMatrix();
    
    ofSetColor(255);
    
}

//minDist should be (simplify/2)^2
void testApp::createTriangleMesh(float minDist){
    triangulatedMesh.clear();
    vector<ofVec3f>& verts = dTriangles.triangleMesh.getVertices();
    vector<ofVec2f>& textureCoords = meshBuilder.getMesh().getTexCoords();
    vector<bool> validVertIndeces;
    
    //do a linear search to find the closest texture coordinate
    for(ofVec3f v : verts){
        int closestTexCoordIndex  = 0;
        float closestTexCoordDistance = FLT_MAX;
        
        for(int j = 0; j < textureCoords.size(); j++){
            float texCoordDist = textureCoords[j].distanceSquared(ofVec2f(v.x, v.y));
            
            if(texCoordDist < closestTexCoordDistance){
                closestTexCoordDistance = texCoordDist;
                closestTexCoordIndex = j;
            }
            //escape if its closer than a minDist so we don't search anymore
           // if(texCoordDist < minDist)
            //    break;
        }
        //copy found verticies into our mesh
        ofVec3f vert = meshBuilder.getMesh().getVertex(closestTexCoordIndex);
        triangulatedMesh.addVertex(vert);
        triangulatedMesh.addTexCoord(meshBuilder.getMesh().getTexCoord(closestTexCoordIndex));
        validVertIndeces.push_back(vert.z < farClip && vert.z > 10);
    }
    
    //copy indices across
    for(int i = 0 ; i < dTriangles.triangleMesh.getNumIndices(); i+=3){
        ofIndexType a,b,c;
        float centerX, centerY;
        
        a = dTriangles.triangleMesh.getIndex(i);
        if(!validVertIndeces[a]) continue;
        
        b = dTriangles.triangleMesh.getIndex(i+1);
        if(!validVertIndeces[b]) continue;
        
        c = dTriangles.triangleMesh.getIndex(i+2);
        if(!validVertIndeces[c]) continue;
        
        //ofVec3f aColor = dTriangles.triangleMesh.getVerticesPointer()[*(dTriangles.triangleMesh.getIndexPointer()+i)];
        //ofVec3f bColor = dTriangles.triangleMesh.getVerticesPointer()[*(dTriangles.triangleMesh.getIndexPointer()+i+1)];
        //ofVec3f cColor = dTriangles.triangleMesh.getVerticesPointer()[*(dTriangles.triangleMesh.getIndexPointer()+i+2)];
        
        ofVec3f aColor = dTriangles.triangleMesh.getVerticesPointer()[a];
        ofVec3f bColor = dTriangles.triangleMesh.getVerticesPointer()[b];
        ofVec3f cColor = dTriangles.triangleMesh.getVerticesPointer()[c];
        
        centerX = (aColor.x + bColor.x + cColor.x)/3;
        centerY = (aColor.y + bColor.y + cColor.y)/3;
        
        ofColor triColor = colorSample(centerX, centerY);
        
        triangulatedMesh.addIndex(dTriangles.triangleMesh.getIndex(i));
        triangulatedMesh.addColor(triColor);
        triangulatedMesh.addIndex(dTriangles.triangleMesh.getIndex(i+1));
        triangulatedMesh.addColor(triColor);
        triangulatedMesh.addIndex(dTriangles.triangleMesh.getIndex(i+2));
        triangulatedMesh.addColor(triColor);
    }
    
}

/*-------------------------------------------------*
 use OpenCv's feature detection to pull out interesting points
 *-------------------------------------------------*/
vector<ofPoint> testApp::featureDetect(){
    goodFeaturesToTrack(ofxCv::toCv(bwImg), harrisPoints, featureMax, (double)featureQuality, (double)featureMinDist);
    return ofxCv::toOf(harrisPoints).getVertices();
}

/*-------------------------------------------------*
 JG bleeeh
 *-------------------------------------------------*/
void testApp::calculateRects(){
    float rectWidth = ofGetWidth()/2;
    float rectHeight = ofGetWidth()/2 * (9./16.);
    videoRect = ofRectangle(rectWidth-vidOffsetX,0,rectWidth,rectHeight);
    meshRect = ofRectangle(rectWidth-vidOffsetX,rectHeight,rectWidth,rectHeight);
	triangulatedRect = ofRectangle(vidOffsetX +10, rectHeight + 10, 
                                   ofGetWidth()-vidOffsetX, 
                                   (ofGetWidth()-vidOffsetX)*(9./16.));
    renderFBO.allocate(rectWidth, rectHeight, GL_RGB, 4);
    previewFBO.allocate(rectWidth, rectHeight, GL_RGB, 4);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if(key == 't'){
        triDraw = !triDraw;
    }
    /*    if( key == ' '){
     if(video.isPlaying())
     video.stop();
     else {
     video.play();
     }
     }*/
}

//--------------------------------------------------------------
//Setup my awesome GUI

void testApp::exit()
{
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui; 
}

void testApp::guiSetup(){
    float dim = 16;
    
    //huge gui setup, put in clean seperate function later
    
    //ofxUi doesn't update your variables for you, so if you add any extra toggles,
    //make sure to add the corresponding vars to the gui catch all function below.
    vector<string> names;
    names.push_back("feature points");
    names.push_back("tracker points");
    
    gui = new ofxUICanvas(0,0,vidOffsetX, ofGetHeight());
    gui->addWidgetDown(new ofxUIRadio( dim, dim, "point arrays", names, OFX_UI_ORIENTATION_HORIZONTAL));
    
    //Feature Detection panel
    gui->addWidgetDown(new ofxUILabel("FEATURE DETECTION", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(new ofxUISpacer(vidOffsetX - labelOffset, 2)); 
    gui->addWidgetDown(new ofxUILabelToggle(vidOffsetX-labelOffset, featureDraw, "draw feature mesh", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUILabelToggle(vidOffsetX-labelOffset, triDraw, "draw triangulation", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIRotarySlider(dim*4, 50, 5000, featureMax, "max number of features")); 
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 0.001, 0.02, featureQuality, "feature quality")); 
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 1, 30, featureMinDist, "feature distance"));
    gui->addWidgetDown(new ofxUISpacer(vidOffsetX - labelOffset, 2)); 
    
    //Tracker panel
    gui->addWidgetDown(new ofxUILabel("TRACKER", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(new ofxUISpacer(vidOffsetX - labelOffset, 2)); 
    gui->addWidgetDown(new ofxUILabelToggle(vidOffsetX-labelOffset, trackerDraw, "draw tracked points", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 0, 100, trackerMaxDist, "max tracked distance"));
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 0, 50, persistance, "tracked point persistance"));
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 0, 100, age, " tracked point age"));
    
    //Particle control
    gui->addWidgetDown(new ofxUILabel("PARTICLE CONTROL", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(new ofxUISpacer(vidOffsetX - labelOffset, 2)); 
    gui->addWidgetDown(new ofxUILabelToggle(vidOffsetX-labelOffset, particleDraw, "draw particle mesh", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 0, 1000, featureMax, "number of particles")); 
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 1, 10, maxSpeed, "max particle speed")); 
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, .001, 1, rep, "repulsion")); 
    gui->addWidgetDown(new ofxUISlider(vidOffsetX-labelOffset, dim, 1, 380, maxDist, "max pull distance")); 
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    
    //debugging graphs to see the size of the tracked points/the feature points frame by frame
    int bufferSize = 256; 
    vector<float> buffer; 
    for(int i = 0; i < bufferSize; i++)
        buffer.push_back(0);
    
    //gui2 = new ofxUICanvas(vidOffsetX, ofGetHeight()-100, ofGetScreenWidth(), ofGetScreenHeight());
    
    gui->addWidgetDown(new ofxUILabel("Feature Points", OFX_UI_FONT_MEDIUM));
    fpSize = (ofxUIMovingGraph *) gui->addWidgetDown(new ofxUIMovingGraph(vidOffsetX-labelOffset, 64, buffer, bufferSize, 0, 1500, "feature points size")); 
    gui->addWidgetDown(new ofxUILabel("Tracked Points", OFX_UI_FONT_MEDIUM));
    tpSize = (ofxUIMovingGraph *) gui->addWidgetDown(new ofxUIMovingGraph(vidOffsetX-labelOffset, 64, buffer, bufferSize, 0, 1500, "tracker points size")); 
    
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
        particles.setNumVerticies(slider->getScaledValue(), maxSpeed);
	}
    else if(name == "repulsion")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        rep = (slider->getScaledValue());
	}
    else if(name == "max particle speed")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        maxSpeed = (slider->getScaledValue());
        
        particles.setSpeed(maxSpeed);
	}
    else if(name == "max pull distance")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        maxDist = (slider->getScaledValue());
	}
    else if(name == "draw tracked points")
	{
		ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
        trackerDraw = toggle->getValue();
	}
    else if(name == "max tracked distance")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        trackerMaxDist = (slider->getScaledValue());
	}
    else if(name == "tracked point persistance")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        persistance = (slider->getScaledValue());
	}
    else if(name == "tracked point age")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        age = (slider->getScaledValue());
	}
    else if(name == "feature points"){
        useTrackedPoints = false;
        cout << "tracked flipped" << endl;
    }
    else if (name == "tracker points"){
        useTrackedPoints = true;
        cout << "tracked flipped" << endl;
    }
    
}

//--------------------------------------------------------------

//Samples a 3x3 color square around the given pixels and returns
//the average color;
//--------------------------------------------------------------

ofColor testApp::colorSample(int x, int  y){
    
    int r, b, g;
    int num = 1;
    ofColor c = img.getColor(x,y);
    r = c.r; b = c.b; g = c.g;
    
    if(x > 0 && x < img.width){
        c = img.getColor(x-1, y);
        r += c.r; b += c.b; g += c.g;
        c = img.getColor(x+1, y);
        r += c.r; b += c.b; g += c.g;
        num += 2;
    }
    if(y>0 && y<img.height){
        c = img.getColor(x, y-1);
        r += c.r; b += c.b; g += c.g;
        c = img.getColor(x, y+1);
        r += c.r; b += c.b; g += c.g;
        num += 2;
    }
    if(x>0 && y > 0 && x < img.width && y < img.height){
        c = img.getColor(x-1, y-1);
        r += c.r; b += c.b; g += c.g;
        c = img.getColor(x+1, y-1);
        r += c.r; b += c.b; g += c.g;
        c = img.getColor(x-1, y+1);
        r += c.r; b += c.b; g += c.g;
        c = img.getColor(x+1, y+1);
        r += c.r; b += c.b; g += c.g;
        num += 4;
    }
    r = r/num;
    g = g/num;
    b = b/num;
    
    return ofColor(r, g, b);
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
  //  particles.addVerticies(1, maxSpeed, ofVec3f(mouseX, mouseY, 0));
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