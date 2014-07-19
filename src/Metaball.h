//
//  Metaball.h
//  amalgam
//
//  Created by Chikashi Miyama on 19/07/14.
//
//

#ifndef __amalgam__Metaball__
#define __amalgam__Metaball__

#include "ofMain.h"
#include "cl.hpp"
#include "Particle.h"
#include "Constant.h"

class Metaball{
    
private:
    cl::Buffer *clParticleSettingBuffer; // GPU
    ParticleSetting particleSetting; // CPU
    
    cl::BufferGL *clBufferGL; // GPU -CL
    ofVbo surfaceVBO; // GPU -GL
    ofVec3f surfacePoints[NUM_PARTICLES]; // CPU
    
    cl::CommandQueue *clQueue;
    
public:
    void setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue);
    void update();
    void draw();
    
};

#endif /* defined(__amalgam__Metaball__) */
