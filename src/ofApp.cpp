#include "ofApp.h"
#include "cl.hpp"
#include "Particle.h"

ofApp::~ofApp(){
    
    if(clUpdateKernelFunctor){
        delete clUpdateKernelFunctor;
        ofLog() << "clUpdateKernelFunctor deleted";
    }
    if (clQueue) {
        delete clQueue;
        ofLog() << "clQueue deleted";
    }
    if(clKernel){
        delete clKernel;
        ofLog() << "clKernel deleted";
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

void ofApp::initParticleSetting(){
    particleSetting.origin[0] = 0.0;
    particleSetting.origin[1] = 0.0;
    particleSetting.origin[2] = 0.0;
    particleSetting.originSpread[0] = 0.0;
    particleSetting.originSpread[1] = 0.0;
    particleSetting.originSpread[2] = 0.0;
    particleSetting.orientation[0] = 0.0;
    particleSetting.orientation[1] = 0.05;
    particleSetting.orientation[2] = 0.0;
    particleSetting.orientationSpread[0] = 0.1;
    particleSetting.orientationSpread[1] = 0.1;
    particleSetting.orientationSpread[2] = 0.1;
    particleSetting.accelerationSpread[0] = 0.0;
    particleSetting.accelerationSpread[1] = 0.0;
    particleSetting.accelerationSpread[2] = 0.0;
    particleSetting.lifeSpan[0] = 200;
    particleSetting.lifeSpan[1] = 200;
    particleSetting.numberOfSpawn = 5;
    particleSetting.spawninIndex = 0;
}

void ofApp::createRandomTable(){
    for (int i = 0; i < NUM_PARTICLES * 3; i++) {
        randomTable[i] = ofRandom(-1.0, 1.0);
    }
}


//--------------------------------------------------------------
void ofApp::setup(){
    
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
    
    ofLog() << "allocate and init GPU buffer";
    clParticleBuffer = new cl::Buffer(*clContext,CL_MEM_READ_WRITE,sizeof(Particle)*NUM_PARTICLES);
    for (int i = 0; i < NUM_PARTICLES; i++){
        //particleBuffer[i].age = 100000; // dead
    }
    
    ofLog() << "create a command Queue";
    clQueue = new cl::CommandQueue(*clContext,default_device);
    clQueue->enqueueWriteBuffer(
                                *clParticleBuffer ,CL_TRUE,0,
                                sizeof(Particle)*NUM_PARTICLES, particleBuffer);

    ofLog() << "allocate and init setting buffer";
    clParticleSettingBuffer = new cl::Buffer(*clContext,CL_MEM_READ_WRITE,sizeof(particleSetting));
    initParticleSetting();
    clQueue->enqueueWriteBuffer(
                                *clParticleSettingBuffer ,CL_TRUE,0,
                                sizeof(ParticleSetting), &particleSetting);
    
    ofLog() << "allocate VBO and setup GL / CL interpolarity";
    
    dotsVBO.setVertexData(dots, NUM_PARTICLES, GL_DYNAMIC_DRAW);
    int vboId = dotsVBO.getVertId();
    clBufferGL = new cl::BufferGL(*clContext, CL_MEM_READ_WRITE, vboId);
    
    ofLog() << "create random Table";
    clRandomTable = new cl::Buffer(*clContext,CL_MEM_READ_WRITE,sizeof(float) * NUM_PARTICLES * 3);
    createRandomTable();
    clQueue->enqueueWriteBuffer(
                                *clRandomTable ,CL_TRUE,0,
                                sizeof(float) * NUM_PARTICLES * 3 , randomTable);
    
    ofLog() << "setup update functor";
    clKernel = new cl::Kernel(*clProgram, "update");
    clUpdateKernelFunctor = new cl::KernelFunctor(*clKernel, *clQueue, cl::NullRange, cl::NDRange(NUM_PARTICLES), cl::NullRange);
    
    ofLog() << "setup finished";
}

//--------------------------------------------------------------
void ofApp::update(){

    particleSetting.spawninIndex += particleSetting.numberOfSpawn;
    if (particleSetting.spawninIndex > NUM_PARTICLES) {
        particleSetting.spawninIndex %= NUM_PARTICLES;
    }
    
    clQueue->enqueueWriteBuffer(
                                *clParticleSettingBuffer ,CL_TRUE,0,
                                sizeof(ParticleSetting), &particleSetting);

    (*clUpdateKernelFunctor)(*clParticleBuffer, *clParticleSettingBuffer, *clBufferGL, *clRandomTable);
    //clQueue->finish();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0, 0, 0);
    camera.setGlobalPosition(0, 1.0, 100);
    camera.setNearClip(0.01);
    camera.setFarClip(10000.0);

    glPointSize(5);
    camera.begin();
    dotsVBO.draw(GL_POINTS, 0, NUM_PARTICLES);
    camera.end();

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
