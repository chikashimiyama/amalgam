#include "ofApp.h"
#include "cl.hpp"
#include "Particle.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    
    
    std::vector<cl::Platform> plats;
    cl::Platform::get(&plats);
    //opencl
    std::clock_t start;
    double duration;
    
    cl_int err = CL_SUCCESS;
    
    ofLog() << "number of platforms found " << plats.size();
    if(plats.size() == 0){
        ofLog() << "no platform found... aborting" << endl;
        abort();
    }
    ofFile clFile;
    clFile.open(ofToDataPath("amalgam.cl"));
    ofBuffer clSource = clFile.readToBuffer();
    std::string srcString = clSource.getText();

    //cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(plats[0])(), 0};

    //GL + CL Interporality
    CGLContextObj kCGLContext = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    cl_context_properties properties[] = { CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup, 0};

    
    clContext = new cl::Context(CL_DEVICE_TYPE_GPU, properties);
    std::vector<cl::Device> devices = clContext->getInfo<CL_CONTEXT_DEVICES>();
    
    cl::Device default_device = devices[0];
    
    cl::Program::Sources source(1, std::make_pair(srcString.c_str(),srcString.size()));
    clProgram = new cl::Program(*clContext, source);
    if(clProgram->build(devices) != CL_SUCCESS){
        ofLog() << " Error building: "<<clProgram->getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device);
        abort();
    }
    
    // allocate a particle buffer in VRAM
    clParticleBuffer = new cl::Buffer(*clContext,CL_MEM_READ_WRITE,sizeof(Particle)*NUM_PARTICLES);

    for (int i = 0; i < NUM_PARTICLES; i++){
        particleBuffer[i].age = 0;
        particleBuffer[i].position[0] = ofRandom(-50, 50);
        particleBuffer[i].position[1] = ofRandom(-50, 50);
        particleBuffer[i].position[2] = ofRandom(-50, 50);
        particleBuffer[i].orientation[0] = ofRandom(-1.0, 1.0);
        particleBuffer[i].orientation[1] = ofRandom(-1.0, 1.0);
        particleBuffer[i].orientation[2] = ofRandom(-1.0, 1.0);
        particleBuffer[i].acceleration[0] = ofRandom(-0.01,0.01);
        particleBuffer[i].acceleration[1] = ofRandom(-0.01,0.01);
        particleBuffer[i].acceleration[2] = ofRandom(-0.01,0.01);
        dots[i].x = ofRandom(-50,50);
        dots[i].y = ofRandom(-50,50);
        dots[i].z = ofRandom(-50,50);
        dotColors[i].r = 1.0;
        dotColors[i].g = ofRandom(0.3, 0.9);
        dotColors[i].b = 1.0;
    }

    dotsVBO.setVertexData(dots, NUM_PARTICLES, GL_DYNAMIC_DRAW);

    int vboId = dotsVBO.getVertId();
    clBufferGL = new cl::BufferGL(*clContext, CL_MEM_READ_WRITE, vboId);
    
    // copy to VRAM
    clQueue = new cl::CommandQueue(*clContext,default_device);
    clQueue->enqueueWriteBuffer(*clParticleBuffer ,CL_TRUE,0,sizeof(Particle)*NUM_PARTICLES, particleBuffer);

    clKernel = new cl::Kernel(*clProgram, "update");
    clUpdateKernelFunctor = new cl::KernelFunctor(*clKernel, *clQueue, cl::NullRange, cl::NDRange(NUM_PARTICLES), cl::NullRange);
    
}

//--------------------------------------------------------------
void ofApp::update(){

    
    (*clUpdateKernelFunctor)(*clParticleBuffer, *clBufferGL);
    clQueue->finish();
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
