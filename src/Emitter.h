//
//  Emitter.h
//  amalgam
//
//  Created by Chikashi Miyama on 19/07/14.
//
//

#ifndef __amalgam__Emitter__
#define __amalgam__Emitter__

#include "ofMain.h"
#include "cl.hpp"
#include "Particle.h"
#include "Constant.h"

#endif /* defined(__amalgam__Emitter__) */

class Emitter{
private:
    float randomTable[SIZE_OF_RANDOM_TABLE];

    cl::CommandQueue *clQueue;
    cl::KernelFunctor *clUpdateKernelFunctor;
    
    // buffers
    cl::Buffer *clParticleBuffer; // GPU
    Particle particleBuffer[NUM_PARTICLES]; // CPU
    
    cl::Buffer *clParticleSettingBuffer; // GPU
    ParticleSetting particleSetting; // CPU
    
    cl::BufferGL *clParticleBufferGL; // GPU -CL
    ofVbo dotsVBO; // GPU -GL
    ofVec3f dots[NUM_PARTICLES]; // CPU
    
    cl::Buffer *clRandomTable;
    cl::Kernel *clKernel;

    // utility function
    void initParticleSetting();
    void createRandomTable();
    void createParameterGroups();
    void updateFromParameters();
    
    // parameters
    ofParameterGroup emitterPG;
    ofParameterGroup originPG, orientationPG, accelerationPG;
    ofParameter<ofVec3f> originP, originSpreadP;
    ofParameter<ofVec3f> orientationP, orientationSpreadP;
    ofParameter<ofVec3f> accelerationP, accelerationSpreadP;
    ofParameter<int> numSpawnP;
    ofParameter<float> isoAttenuationP;
    
    ofParameterGroup forcePG;
    ofParameter<ofVec3f> tornadeP;
    ofParameter<ofVec3f> turbulenceP;
    
    
public:
    ~Emitter();
    void setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue);
    void update();
    void draw();

    ofParameterGroup getEmitterParameterGroup();
    ofParameterGroup getForceParameterGroup();

    cl::BufferGL *getParticleBufferGL();

};

inline ofParameterGroup Emitter::getEmitterParameterGroup(){
    return emitterPG;
}

inline ofParameterGroup Emitter::getForceParameterGroup(){
    return forcePG;
}

inline cl::BufferGL *Emitter::getParticleBufferGL(){
    return clParticleBufferGL;
}