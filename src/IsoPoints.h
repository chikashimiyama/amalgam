//
//  IsoPoints.h
//  amalgam
//
//  Created by Chikashi Miyama on 30/07/14.
//
//

#ifndef __amalgam__IsoPoints__
#define __amalgam__IsoPoints__

#include "ofMain.h"
#include "cl.hpp"
#include "Particle.h"
#include "Constant.h"
#include "SceneComponent.h"

class IsoPoints : public SceneComponent{
 
private:
    cl::CommandQueue *clQueue;
    
#pragma mark ISOpoints
    IsoPoint isoPoints[NUM_ISO_POINTS]; // CPU
    cl::Buffer *clIsoPoints; // GPU -CL

#pragma mark debugging
    ofVec3f isoPointsVerticies[NUM_ISO_POINTS];
    ofFloatColor isoPointsColors[NUM_ISO_POINTS];
    ofVbo isoPointsVBO; // GPU - GL for test

#pragma mark Kernels
    cl::Kernel *clKernelUpdateIsoPoints;
    cl::KernelFunctor *clUpdateIsoPointsFunctor;

#pragma mark utility
    void createMatrix();
        
    public:

    ~IsoPoints();
    void setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue);
    void update(cl::BufferGL *clParticleBufferGL );
    void draw();
    cl::Buffer *getIsoPointsBuffer();
};

inline cl::Buffer *IsoPoints::getIsoPointsBuffer(){
    return clIsoPoints;
}

#endif /* defined(__amalgam__IsoPoints__) */
