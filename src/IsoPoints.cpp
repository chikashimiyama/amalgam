//
//  IsoPoints.cpp
//  amalgam
//
//  Created by Chikashi Miyama on 30/07/14.
//
//

#include "IsoPoints.h"

IsoPoints::~IsoPoints(){
    if(clUpdateIsoPointsFunctor)delete clUpdateIsoPointsFunctor;
    if(clKernelUpdateIsoPoints)delete clKernelUpdateIsoPoints;
}

void IsoPoints::createMatrix(){
    
    int ct = 0;
    for(int i =0;i<RESOLUTION;i++){
        for(int j=0;j<RESOLUTION;j++){
            for(int k=0;k<RESOLUTION;k++){
                isoPoints[ct].x = static_cast<cl_float>((i-OFFSET) * STEP);
                isoPoints[ct].y = static_cast<cl_float>((j-OFFSET) * STEP);
                isoPoints[ct].z = static_cast<cl_float>((k-OFFSET) * STEP);
                
                isoPointsVerticies[ct].x = isoPoints[ct].x;
                isoPointsVerticies[ct].y = isoPoints[ct].y;
                isoPointsVerticies[ct].z = isoPoints[ct].z;
                isoPointsColors[ct].r = 0.0;
                isoPointsColors[ct].g = 0.0;
                isoPointsColors[ct].b = 0.0;
                //ofLog() <<ct << " " << static_cast<cl_float>(i * STEP) << " " << isoPoints[ct].y << " "<<  isoPoints[ct].z;
                
                ct++;
            }
        }
    }
}

void IsoPoints::setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue){
    
    // create 3d matrix of iso points
    createMatrix();

    // store a pointer to clQueue
    IsoPoints::clQueue = clQueue;

    // alloc buffer
    clIsoPoints = new cl::Buffer(*clContext, CL_MEM_READ_WRITE, sizeof(IsoPoint) * NUM_ISO_POINTS);
    clQueue->enqueueWriteBuffer(*clIsoPoints ,CL_TRUE,0,sizeof(IsoPoint) * NUM_ISO_POINTS, isoPoints);

    // extra vbo for debugging
    isoPointsVBO.setVertexData(isoPointsVerticies, NUM_ISO_POINTS, GL_STATIC_DRAW); // the position is fixed
    isoPointsVBO.setColorData(isoPointsColors, NUM_ISO_POINTS, GL_DYNAMIC_DRAW); // the color will be modified
    
    
    clKernelUpdateIsoPoints = new cl::Kernel(*clProgram, "updateIsoPoints");
    clUpdateIsoPointsFunctor = new cl::KernelFunctor(
                                                     *clKernelUpdateIsoPoints, *clQueue,
                                                     cl::NullRange,
                                                     cl::NDRange(NUM_ISO_POINTS),
                                                     cl::NullRange);
    drawFlag = false;
}

void IsoPoints::update(cl::BufferGL *clParticleBufferGL ){
    cl::Event event;
    
    (*clUpdateIsoPointsFunctor)(*clIsoPoints ,*clParticleBufferGL, &event);
    event.wait();

    if(drawFlag){
        clQueue->enqueueReadBuffer(*clIsoPoints ,CL_TRUE,0,sizeof(IsoPoint) * NUM_ISO_POINTS, isoPoints, NULL, &event);
        event.wait();
        for (int i = 0; i < NUM_ISO_POINTS; i++) {
            isoPointsColors[i].r = isoPoints[i].isoValue * 1000.0;
        }
        isoPointsVBO.updateColorData(isoPointsColors, NUM_ISO_POINTS);
    }
}

void IsoPoints::draw(){
    if(drawFlag){
        isoPointsVBO.draw(GL_POINTS, 0, NUM_ISO_POINTS);
    }
}
