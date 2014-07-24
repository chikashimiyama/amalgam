#include "ofApp.h"
#include "cl.hpp"
#include "Particle.h"

ofApp::~ofApp(){
    if (clQueue) {
        delete clQueue;
        ofLog() << "clQueue deleted";
    }
    if (clProgram) {
        delete clProgram;
        ofLog() << "clProgram deleted";
    }
    if(clContext){
        delete clContext;
        ofLog() << "clContext deleted";
    }
}

void ofApp::setupGui(){
    
    panel.setup("Parameters");
    panel.add(emitter.getParameterGroup());
}

void ofApp::setup(){
    
    ofSetSmoothLighting(true);
    
    cl_int err = CL_SUCCESS;

    // look for platform
    ofLog() << "get list of platorms";
    std::vector<cl::Platform> plats;
    cl::Platform::get(&plats);
    ofLog() << "number of platforms found " << plats.size();
    if(plats.size() == 0){
        ofLog() << "no platform found... aborting" << endl;
        abort();
    }
    
    // create context
    ofLog()  << "create OpenCL context";
    CGLContextObj kCGLContext = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    cl_context_properties properties[] = { CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup, 0};
    clContext = new cl::Context(CL_DEVICE_TYPE_GPU, properties);
    std::vector<cl::Device> devices = clContext->getInfo<CL_CONTEXT_DEVICES>();
    if(devices.size() == 0){
        ofLog() << "no GPU device found... aborting";
        abort();
    }
    cl::Device default_device = devices[0];
    size_t maxWorkGroupSize;
    size_t globalMemSize;
    size_t maxWorkItemDimensions;
    size_t* maxWorkItemSizes;
    default_device.getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxWorkGroupSize);
    default_device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &globalMemSize);
    default_device.getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, &maxWorkItemDimensions);
    maxWorkItemSizes = new size_t[maxWorkItemDimensions];
    default_device.getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, maxWorkItemSizes);
    
    ofLog() << "Max work group size:" << maxWorkGroupSize;
    ofLog() << "Global memory size:" << globalMemSize;
    ofLog() << "Max Work Item Dimensions:" << maxWorkItemDimensions;
    ofLog() << "Max Work Item Sizes:";
    for(int i = 0; i < maxWorkItemDimensions; i ++){
        ofLog() << " dim" << i << ":" << maxWorkItemSizes[i];
    }
    
    delete maxWorkItemSizes;
    
    // open CL file
    ofLog() << "load CL program";
    ofFile clFile;
    clFile.open(ofToDataPath("amalgam.cl"));
    ofBuffer clSource = clFile.readToBuffer();
    std::string srcString = clSource.getText();
    cl::Program::Sources source(1, std::make_pair(srcString.c_str(),srcString.size()));
    clProgram = new cl::Program(*clContext, source);
    if(clProgram->build(devices) != CL_SUCCESS){
        ofLog() << " Error building: "<<clProgram->getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device);
        abort();
    }
    ofLog() << "CL program successfuly built";
    
    clQueue = new cl::CommandQueue(*clContext,default_device);
    
    //gui
    emitter.setup(clContext, clProgram, clQueue);
    metaball.setup(clContext, clProgram, clQueue);
    
    setupGui();
    
    texMaterial.setup();
    ofLog() << "setup finished";
}



//--------------------------------------------------------------
void ofApp::update(){
    
    emitter.update();
    metaball.update(emitter.getParticleBufferGL());
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    //texMaterial.customDraw();
   
    camera.setPosition(300, 300, 600);
    camera.lookAt(ofVec3f(0,0,0));
    camera.setNearClip(0.01);
    camera.setFarClip(10000.0);

    camera.begin();
    //emitter.draw();
    metaball.draw();

    camera.end();
    panel.draw();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
