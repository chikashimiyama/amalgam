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
    panel.add(emitter.getEmitterParameterGroup());
    panel.add(emitter.getForceParameterGroup());
}


void ofApp::setupScene(){
    
    modelMatrix.makeIdentityMatrix();
    
    ofVec3f eyePosition = ofVec3f(0.0, 200.0, 800.0);
    ofVec3f lookAt = ofVec3f(0.0, 0.0, 0.0);
    
    viewMatrix.makeLookAtViewMatrix(eyePosition, lookAt, ofVec3f(0.0, 1.0, 0.0));
    projectionMatrix.makePerspectiveMatrix(60, 1.3333, 0.1, 10000);
    
    modelViewMatrix = modelMatrix * viewMatrix;
    MVP = modelMatrix * viewMatrix * projectionMatrix;
    
    normalMatrix = reduceMatrixFrom4to3(modelViewMatrix);
    
    lightPosition = ofVec4f(10.0, 200.0, 700, 1.0);
    lightPosition = lightPosition * viewMatrix; // eye coordinate
}

void ofApp::setupCL(){
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
    ofFile clFile;
    clFile.open(ofToDataPath("amalgam.cl"));
    ofBuffer clSource = clFile.readToBuffer();
    std::string srcString = clSource.getText();
    cl::Program::Sources source(1, std::make_pair(srcString.c_str(),srcString.size()));
    clProgram = new cl::Program(*clContext, source);
    if(clProgram->build(devices) != CL_SUCCESS){
        abort();
    }
    clQueue = new cl::CommandQueue(*clContext,default_device);
    
}

void ofApp::setupShader(){
    shader.load("shaders/Phong");
}

void ofApp::setup(){
    panelFlag = true;
    
    setupCL();
    emitter.setup(clContext, clProgram, clQueue);
    isoPoints.setup(clContext, clProgram, clQueue);
    metaball.setup(clContext, clProgram, clQueue);
    setupScene();
    setupGui();
    setupShader();
    
    ofLog() << "setup finished";
}



//--------------------------------------------------------------
void ofApp::update(){
    
    emitter.update();
    isoPoints.update(emitter.getParticleBufferGL());
    metaball.update(emitter.getParticleBufferGL(), isoPoints.getIsoPointsBuffer());

    //shader update
    shader.begin();
    shader.setUniform4fv("LightPosition", lightPosition.getPtr());
    shader.setUniform3f("Kd", 1.0, 1.0, 1.0);
    shader.setUniform3f("Ld", 1.0, 1.0, 1.0);
    shader.setUniformMatrix4f("ModelViewMatrix",modelViewMatrix);
    shader.setUniformMatrix4f("ProjectionMatrix",projectionMatrix);
    GLuint prog = shader.getProgram();
    int loc = glGetUniformLocation(prog, "NormalMatrix");
    if(loc < 0){
        ofLog() << "no such uniform, NormalMatrix";
    }else{
        float rawMatrix[9] =
        {   normalMatrix.a, normalMatrix.b, normalMatrix.c,
            normalMatrix.d, normalMatrix.e, normalMatrix.f,
            normalMatrix.g, normalMatrix.h, normalMatrix.i};
        glUniformMatrix3fv(loc, 1, GL_FALSE, rawMatrix);
    }
    shader.setUniformMatrix4f("MVP", MVP);
    shader.end();
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofClear(0); // why should I do it manually? 
    ofEnableDepthTest();
    shader.begin();
        emitter.draw();
        metaball.draw();
        isoPoints.draw();
        shader.end();
    ofDisableDepthTest();
    
    if(panelFlag) panel.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    switch(key){
        case 'p':
            panelFlag = !panelFlag;
            break;
        case 'm':
            metaball.toggleDrawFlag();
            break;
        case 'i':
            isoPoints.toggleDrawFlag();
            break;
        case 'e':
            emitter.toggleDrawFlag();
    }
    
}


ofMatrix3x3 ofApp::reduceMatrixFrom4to3(ofMatrix4x4 mat4){
    ofMatrix3x3 mat3;
    float *rawPtr = mat4.getPtr();
    mat3.a = rawPtr[0];
    mat3.b = rawPtr[1];
    mat3.c = rawPtr[2];
    mat3.d = rawPtr[4];
    mat3.e = rawPtr[5];
    mat3.f = rawPtr[6];
    mat3.g = rawPtr[8];
    mat3.h = rawPtr[9];
    mat3.i = rawPtr[10];
    return mat3;
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
