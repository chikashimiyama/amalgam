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
    cameraPG.setName("camera");
    cameraPG.add(azimuthP.set("azimuth", 0.0, 0.0, PI * 2.0));
    cameraPG.add(elevationP.set("elevation", 0.05, -PI, PI));
    cameraPG.add(distanceP.set("distance", 700, 0.0, 1000));

    ofxGuiSetDefaultHeight(10);
    panel.setup("Parameters");
    panel.add(emitter.getEmitterParameterGroup());
    panel.add(emitter.getForceParameterGroup());
    panel.add(metaball.getParameterGroup());
    panel.add(cameraPG);
}


void ofApp::setupScene(){
    
    modelMatrix.makeIdentityMatrix();
    projectionMatrix.makePerspectiveMatrix(60, 1.3333, 0.1, 10000);
    

    
    normalMatrix = reduceMatrixFrom4to3(modelViewMatrix);
    
    lightPosition = ofVec4f(500.0, 700.0, 700, 1.0);
    lightPosition = lightPosition * viewMatrix; // eye coordinate

    cubeImage[0].loadImage("posx.jpg");
    cubeImage[1].loadImage("posy.jpg");
    cubeImage[2].loadImage("posz.jpg");
    cubeImage[3].loadImage("negx.jpg");
    cubeImage[4].loadImage("negy.jpg");
    cubeImage[5].loadImage("negz.jpg");
    
    
}

void ofApp::setupCL(){
    cl_int err = CL_SUCCESS;
    
    // look for platform
    ofLog() << "get list of platorms";
    std::vector<cl::Platform> plats;
    cl::Platform::get(&plats);
    ofLog() << "number of platforms found:" << plats.size();
    if(plats.size() == 0){
        ofLog() << "no platform found... aborting" << endl;
        abort();
    }
    
    // create context
    ofLog()  << "create OpenCL context";
    CGLContextObj kCGLContext = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    cl_context_properties properties[] = { CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup, 0};
    clContext = new cl::Context(CL_DEVICE_TYPE_DEFAULT, properties);
    std::vector<cl::Device> devices = clContext->getInfo<CL_CONTEXT_DEVICES>();
    ofLog() << "number of devices found:" << devices.size();
    
    if(devices.size() == 0){
        ofLog() << "no GPU device found... aborting";
        abort();
    }
    
    cl::Device default_device = devices[0];
    
    
    size_t maxWorkGroupSize;
    size_t globalMemSize;
    size_t maxWorkItemDimensions;
    size_t* maxWorkItemSizes;
    char vendor[32];
    
    default_device.getInfo(CL_DEVICE_VENDOR, vendor);
    default_device.getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxWorkGroupSize);
    default_device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &globalMemSize);
    default_device.getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, &maxWorkItemDimensions);
    maxWorkItemSizes = new size_t[maxWorkItemDimensions];
    
    ofLog() << "Device Vendor:" << default_device.getInfo<CL_DEVICE_VENDOR>();
    ofLog() << "Device Name:" << default_device.getInfo<CL_DEVICE_NAME>();

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
    
    ofVec3f cameraPos = poltocar(distanceP.get(), elevationP.get(), azimuthP.get());
    
    viewMatrix.makeLookAtViewMatrix(cameraPos, ofVec3f(0, 0, 0), ofVec3f(0.0, 1.0, 0.0));
    modelViewMatrix = modelMatrix * viewMatrix;
    MVP = modelMatrix * viewMatrix * projectionMatrix;
    
    
    emitter.update();
    isoPoints.update(emitter.getParticleBufferGL());
    metaball.update(emitter.getParticleBufferGL(), isoPoints.getIsoPointsBuffer());

    
    //shader update
    shader.begin();
//    shader.setUniformTexture("tex0", cubeImage[0].getTextureReference(), 0);
//    shader.setUniformTexture("tex1", cubeImage[1].getTextureReference(), 1);
//    shader.setUniformTexture("tex2", cubeImage[2].getTextureReference(), 2);
//    shader.setUniformTexture("tex3", cubeImage[3].getTextureReference(), 3);
//    shader.setUniformTexture("tex4", cubeImage[4].getTextureReference(), 4);
//    shader.setUniformTexture("tex5", cubeImage[5].getTextureReference(), 5);

    shader.setUniform3f("LightIntensity", 1.0, 1.0, 1.0);
    shader.setUniform4fv("LightPosition", lightPosition.getPtr());
    shader.setUniform3f("Ka", 0.5, 0.5, 0.5);
    shader.setUniform3f("Kd", 0.3, 0.3, 0.3);
    shader.setUniform3f("Ks", 0.5, 0.5, 0.5);
    shader.setUniform1f("Shininess", 1.0);

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
    ofLog() << ofGetFrameRate();
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
