//
//  Metaball.cpp
//  amalgam
//
//  Created by Chikashi Miyama on 19/07/14.
//
//

#include "Metaball.h"

Metaball::~Metaball(){

    if(clTriangleSurfaceBufferGL)delete clTriangleSurfaceBufferGL;
    if(clInspector)delete clInspector;
}

int Metaball::getPointPos(int x, int y, int z){
    return x+y*RESOLUTION+z*RESOLUTION*RESOLUTION;

}


void Metaball::setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue){
    PG.setName("Metaball");
    isoThresholdP.set("isoThreshold", 0.0004, 0.0001, 0.0010);
    PG.add(isoThresholdP);
    
    Metaball::clQueue = clQueue;
    clInspector = new cl::Buffer(*clContext, CL_MEM_READ_WRITE, sizeof(Inspector));
    
    //triangle vbo;
    triangleSurfaceVBO.setVertexData(triangleSurface, NUM_ISO_POINTS, GL_DYNAMIC_DRAW); // modified and drawn
    triangleSurfaceVBO.setNormalData(triangleSurfaceNormal, NUM_ISO_POINTS, GL_DYNAMIC_DRAW); // modifieed and drawn

    int vboId = triangleSurfaceVBO.getVertId();
    int vboNormalId = triangleSurfaceVBO.getNormalId();
    clTriangleSurfaceBufferGL = new cl::BufferGL(*clContext, CL_MEM_READ_WRITE, vboId);
    clTriangleSurfaceNormalBufferGL = new cl::BufferGL(*clContext, CL_MEM_READ_WRITE, vboNormalId);
    
    clKernelCreateIsoSurface = new cl::Kernel(*clProgram, "createIsoSurface");
    clCreateIsoSurfaceFunctor = new cl::KernelFunctor(*clKernelCreateIsoSurface,
                                                      *clQueue,
                                                      cl::NullRange,
                                                      cl::NDRange(RESOLUTION_MINUS_ONE, RESOLUTION_MINUS_ONE, RESOLUTION_MINUS_ONE),
                                                      cl::NullRange);
    


    drawFlag = true;
}

void Metaball::update(cl::BufferGL *clParticleBufferGL, cl::Buffer *clIsoPoints){

    
    cl::Event event;
    inspector.numberOfValidCubes = 0;
    inspector.numberOfValidPoints = 0;
    inspector.isoThreshold = isoThresholdP.get();
    
    clQueue->enqueueWriteBuffer(*clInspector, GL_TRUE, 0, sizeof(Inspector), &inspector, NULL, &event);
    event.wait();

    (*clCreateIsoSurfaceFunctor)(*clIsoPoints,
                                 *clTriangleSurfaceBufferGL,
                                 *clTriangleSurfaceNormalBufferGL,
                                 *clInspector, &event);
    event.wait();
    // get number of points to be drawn
    clQueue->enqueueReadBuffer(*clInspector ,CL_TRUE,0,sizeof(Inspector), &inspector, NULL, &event);
    event.wait();
    numValidPoints = inspector.numberOfValidPoints;

}

void Metaball::draw(){
    if(drawFlag){
        triangleSurfaceVBO.draw(GL_TRIANGLES, 0, numValidPoints);
    }
}


