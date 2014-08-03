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
#include "SceneComponent.h"
#endif /* defined(__amalgam__Emitter__) */

typedef struct Particle{
    ofVec3f orientation;
    ofVec3f acceleration;
} Particle;

class Emitter : public SceneComponent {
private:

#pragma mark randomTable
    float randomTable[SIZE_OF_RANDOM_TABLE];
    
#pragma mark particleBuffers
    ofVec3f particlePosition[NUM_PARTICLES];
    Particle particles[NUM_PARTICLES]; // CPU
    ofVbo particleVbo; // GPU -GL
    cl::BufferGL *clParticleBufferGL; // GPU -CL

#pragma mark openCL
    cl::CommandQueue *clQueue;
    cl::KernelFunctor *clUpdateKernelFunctor;

    cl::Buffer *clRandomTable;
    cl::Kernel *clKernel;

#pragma mark utility
    void setupParticleSetting();
    void createRandomTable();
    void createParameterGroups();
    void updateFromParameters();
    
#pragma mark parameters
    ofParameterGroup emitterPG;
    ofParameterGroup originPG, orientationPG, accelerationPG;
    ofParameter<ofVec3f> originP, originSpreadP;
    ofParameter<ofVec3f> orientationP, orientationSpreadP;
    ofParameter<ofVec3f> accelerationP, accelerationSpreadP;
    ofParameter<int> numSpawnP;
    ofParameter<float> isoAttenuationP;
    
    ofParameterGroup forcePG;
    ofParameter<ofVec3f> turbulenceP;
    
public:
    ~Emitter();
    void setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue);
    void updateParticlePosition();
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