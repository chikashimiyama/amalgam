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
    int numValidPoints;

    /*** Shader ***/
    ofShader shader;
    
    ofVec3f lightPosition;
    ofMatrix4x4 modelMatrix;
    ofMatrix4x4 viewMatrix;
    ofMatrix4x4 projectionMatrix;

    ofMatrix4x4 modelViewMatrix;
    ofMatrix4x4 MVP;

    ofMatrix3x3 normalMatrix;
    
    
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
    ofVec2f triangleSurfaceTexture[NUM_ISO_POINTS];

    cl::BufferGL *clTriangleSurfaceBufferGL; // GPU -CL
    cl::BufferGL *clTriangleSurfaceNormalBufferGL;
    cl::BufferGL *clTriangleSurfaceTextureBufferGL;
    
    ofVbo triangleSurfaceVBO; // GPU -GL
    
    ofLight light;
    
    
    cl::Kernel *clKernelUpdateIsoPoints;
    cl::KernelFunctor *clUpdateIsoPointsFunctor;
    
    cl::Kernel *clKernelCreateIsoSurface;
    cl::KernelFunctor *clCreateIsoSurfaceFunctor;

    
    
    cl::CommandQueue *clQueue;
    int getPointPos(int x, int y, int z);
    void createMatrix();
    void sceneSetting();
    void createTriangleVbo();
    ofMatrix3x3 reduceMatrixFrom4to3(ofMatrix4x4 mat4);
    bool metaballFlag;
    bool isoPointsFlag;
    ofMaterial material;
    
public:
    ~Metaball();
    void setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue);
    void update(cl::BufferGL *clParticleBufferGL);
    void draw();
    
    void toggleMetaballFlag();
    void toggleIsoPointsFlag();
};
#endif /* defined(__amalgam__Metaball__) */
