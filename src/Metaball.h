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
    int numValidIndicies;

    /*** Shader ***/
    ofShader shader;
    
    
    /*** OpenCL Buffers ***/
    IsoPoint isoPoints[NUM_ISO_POINTS]; // CPU
    cl::Buffer *clIsoPoints; // GPU -CL
    
    Inspector inspector;
    cl::Buffer *clInspector;
    
    // for debugging
    ofVec3f isoPointsVerticies[NUM_ISO_POINTS];
    ofFloatColor isoPointsColors[NUM_ISO_POINTS];
    ofVbo isoPointsVBO; // GPU - GL for test
    
    ofVec3f triangleSurface[NUM_ISO_POINTS]; // CPU
    ofVec3f triangleSurfaceNormal[NUM_ISO_POINTS];
    
    cl::BufferGL *clTriangleSurfaceBufferGL; // GPU -CL
    cl::BufferGL *clTriangleSurfaceNormalBufferGL;
    
    ofVbo triangleSurfaceVBO; // GPU -GL
    
    ofLight light;
    
    
    cl::Kernel *clKernelUpdateIsoPoints;
    cl::KernelFunctor *clUpdateIsoPointsFunctor;
    
    cl::Kernel *clKernelCreateIsoSurface;
    cl::KernelFunctor *clCreateIsoSurfaceFunctor;

    
    
    cl::CommandQueue *clQueue;
    int getPointPos(int x, int y, int z);
    void createMatrix();
    void createTriangleVbo();

    
public:
    ~Metaball();
    void setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue);
    void update(cl::BufferGL *clParticleBufferGL);
    void draw();
    
};
#endif /* defined(__amalgam__Metaball__) */
