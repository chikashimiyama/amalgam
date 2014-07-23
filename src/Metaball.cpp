//
//  Metaball.cpp
//  amalgam
//
//  Created by Chikashi Miyama on 19/07/14.
//
//

#include "Metaball.h"

Metaball::~Metaball(){
    if(clUpdateIsoPointsFunctor)delete clUpdateIsoPointsFunctor;
    if(clKernelUpdateIsoPoints)delete clKernelUpdateIsoPoints;
    if(clTriangleSurfaceBufferGL)delete clTriangleSurfaceBufferGL;
    if(clInspector)delete clInspector;
}

int Metaball::getPointPos(int x, int y, int z){
    return x+y*RESOLUTION+z*RESOLUTION*RESOLUTION;

}

void Metaball::createMatrix(){

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

void Metaball::createTriangleVbo(){
//    int ct = 0;
//    for(int i =0;i<NUM_ISO_POINTS;i++){
//        triangleSurface[i].x = 10.0;
//        triangleSurface[i].y = 20.0;
//        triangleSurface[i].z = 3.0;
//    }
}

void Metaball::setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue){
    
    // copy pointer to queue
    Metaball::clQueue = clQueue;
    
    // alloc buffer
    createMatrix();
    clIsoPoints = new cl::Buffer(*clContext, CL_MEM_READ_WRITE, sizeof(IsoPoint) * NUM_ISO_POINTS);
    clQueue->enqueueWriteBuffer(*clIsoPoints ,CL_TRUE,0,sizeof(IsoPoint) * NUM_ISO_POINTS, isoPoints);
    
    clInspector = new cl::Buffer(*clContext, CL_MEM_READ_WRITE, sizeof(Inspector));

    
    // extra vbo for debugging
    isoPointsVBO.setVertexData(isoPointsVerticies, NUM_ISO_POINTS, GL_STATIC_DRAW);
    isoPointsVBO.setColorData(isoPointsColors, NUM_ISO_POINTS, GL_DYNAMIC_DRAW);
    
    
    //triangle vbo;
    createTriangleVbo();
    triangleSurfaceVBO.setVertexData(triangleSurface, NUM_ISO_POINTS*3, GL_DYNAMIC_DRAW);
    int vboId = triangleSurfaceVBO.getVertId();
    clTriangleSurfaceBufferGL = new cl::BufferGL(*clContext, CL_MEM_READ_WRITE, vboId);
    
    
    clKernelUpdateIsoPoints = new cl::Kernel(*clProgram, "updateIsoPoints");
    clUpdateIsoPointsFunctor = new cl::KernelFunctor(
                                                     *clKernelUpdateIsoPoints, *clQueue,
                                                     cl::NullRange,
                                                     cl::NDRange(NUM_ISO_POINTS),
                                                     cl::NullRange);
    
    clKernelCreateIsoSurface = new cl::Kernel(*clProgram, "createIsoSurface");
    clCreateIsoSurfaceFunctor = new cl::KernelFunctor(*clKernelCreateIsoSurface,
                                                      *clQueue,
                                                      cl::NullRange,
                                                      cl::NDRange(RESOLUTION_MINUS_ONE, RESOLUTION_MINUS_ONE, RESOLUTION_MINUS_ONE),
                                                      cl::NullRange);
    
}

void Metaball::update(cl::BufferGL *clParticleBufferGL){

    cl::Event event;

    inspector.numberOfValidCubes = 0;
    inspector.numberOfValidIndicies = 0;
    clQueue->enqueueWriteBuffer(*clInspector, GL_TRUE, 0, sizeof(Inspector), &inspector, NULL, &event);
    event.wait();
    
    (*clUpdateIsoPointsFunctor)(*clIsoPoints ,*clParticleBufferGL, &event);
    event.wait();
    
    (*clCreateIsoSurfaceFunctor)(*clIsoPoints, *clTriangleSurfaceBufferGL, *clInspector, &event);
    event.wait();
    
    clQueue->enqueueReadBuffer(*clInspector ,CL_TRUE,0,sizeof(Inspector), &inspector, NULL, &event);
    event.wait();
    numValidIndicies = inspector.numberOfValidIndicies;

//    // copy vram to ram
//    clQueue->enqueueReadBuffer(*clIsoPoints ,CL_TRUE,0,sizeof(IsoPoint) * NUM_ISO_POINTS, isoPoints, NULL, &event);
//    event.wait();
//    //ofLog() << "number of valid cubes: " << inspector.numberOfValidCubes << " number of valid indicies: " << numValidIndicies;
//    // isoValue to color
//    for (int i = 0; i < NUM_ISO_POINTS; i++) {
//        isoPointsColors[i].r = isoPoints[i].isoValue * 1000.0;
//    }
//    isoPointsVBO.updateColorData(isoPointsColors, NUM_ISO_POINTS);
    
}

void Metaball::draw(){
    glEnable(GL_DEPTH_TEST);
    glPointSize(2);
    ofSetColor(255, 255, 255);
    triangleSurfaceVBO.draw(GL_TRIANGLES, 0, numValidIndicies/3);
    glPointSize(1);
    glDisable(GL_DEPTH_TEST);

    //isoPointsVBO.draw(GL_POINTS, 0, NUM_ISO_POINTS);
    
}

