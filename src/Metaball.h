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
#include "SceneComponent.h"

class Metaball : public SceneComponent {
    
private:

#pragma mark Inspector
    Inspector inspector;
    cl::Buffer *clInspector;
    int numValidPoints;
    
#pragma mark VBO
    ofVbo triangleSurfaceVBO; // GPU -GL

#pragma mark Vertex
    ofVec3f triangleSurface[NUM_ISO_POINTS]; // CPU
    cl::BufferGL *clTriangleSurfaceBufferGL; // GPU -CL

#pragma mark Normal
    ofVec3f triangleSurfaceNormal[NUM_ISO_POINTS]; // CPU
    cl::BufferGL *clTriangleSurfaceNormalBufferGL; // GPU - CL

#pragma mark Kernels
    cl::Kernel *clKernelCreateIsoSurface;
    cl::KernelFunctor *clCreateIsoSurfaceFunctor;

#pragma mark CL Queue
    cl::CommandQueue *clQueue;
    
#pragma mark utility
    int getPointPos(int x, int y, int z);
    void sceneSetting();
    
#pragma mark parameters
    ofParameterGroup PG;
    ofParameter<float>isoThresholdP;

public:
    ~Metaball();
    ofParameterGroup getParameterGroup();
    void setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue);
    void update(cl::BufferGL *clParticleBufferGL, cl::Buffer *clIsoPoints);
    void draw();
};

inline ofParameterGroup Metaball::getParameterGroup(){
    return PG;
}
#endif /* defined(__amalgam__Metaball__) */
