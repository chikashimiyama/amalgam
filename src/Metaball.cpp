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

ofMatrix3x3 Metaball::reduceMatrixFrom4to3(ofMatrix4x4 mat4){
    ofMatrix3x3 mat3;
    float *rawPtr = mat4.getPtr();
    mat3.a = rawPtr[0];
    mat3.b = rawPtr[1];
    mat3.c = rawPtr[2];
    mat3.d = rawPtr[4];
    mat3.e = rawPtr[5];
    mat3.f = rawPtr[6];
    mat3.g = rawPtr[8];
    mat3.h = rawPtr[9];
    mat3.i = rawPtr[10];
    return mat3;
}

void Metaball::sceneSetting(){
    modelMatrix.makeIdentityMatrix();
    
    ofVec3f eyePosition = ofVec3f(0.0, 200.0, 800.0);
    ofVec3f lookAt = ofVec3f(0.0, 0.0, 0.0);
    
    viewMatrix.makeLookAtViewMatrix(eyePosition, lookAt, ofVec3f(0.0, 1.0, 0.0));
    projectionMatrix.makePerspectiveMatrix(60, 1.3333, 0.1, 10000);
    
    modelViewMatrix = modelMatrix * viewMatrix;
    MVP = modelMatrix * viewMatrix * projectionMatrix;

    normalMatrix = reduceMatrixFrom4to3(modelViewMatrix);

    lightPosition = ofVec4f(500.0, 200.0, -400, 1.0);
    lightPosition = lightPosition * viewMatrix; // eye coordinate
}

void Metaball::setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue){
    
    
    sceneSetting();
    shader.load("shaders/Phong");
    
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
    triangleSurfaceVBO.setVertexData(triangleSurface, NUM_ISO_POINTS, GL_DYNAMIC_READ);
    triangleSurfaceVBO.setNormalData(triangleSurfaceNormal, NUM_ISO_POINTS, GL_DYNAMIC_READ);

    int vboId = triangleSurfaceVBO.getVertId();
    int vboNormalId = triangleSurfaceVBO.getNormalId();
    clTriangleSurfaceBufferGL = new cl::BufferGL(*clContext, CL_MEM_READ_WRITE, vboId);
    clTriangleSurfaceNormalBufferGL = new cl::BufferGL(*clContext, CL_MEM_READ_WRITE, vboNormalId);
    
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
    

    isoPointsFlag = false;
    metaballFlag = true;

}

void Metaball::update(cl::BufferGL *clParticleBufferGL){

    cl::Event event;

    inspector.numberOfValidCubes = 0;
    inspector.numberOfValidPoints = 0;
    clQueue->enqueueWriteBuffer(*clInspector, GL_TRUE, 0, sizeof(Inspector), &inspector, NULL, &event);
    event.wait();
    
    (*clUpdateIsoPointsFunctor)(*clIsoPoints ,*clParticleBufferGL, &event);
    event.wait();
    
    (*clCreateIsoSurfaceFunctor)(*clIsoPoints,
                                 *clTriangleSurfaceBufferGL,
                                 *clTriangleSurfaceNormalBufferGL,
                                 *clInspector, &event);
    event.wait();
    
    clQueue->enqueueReadBuffer(*clInspector ,CL_TRUE,0,sizeof(Inspector), &inspector, NULL, &event);
    event.wait();
    numValidPoints = inspector.numberOfValidPoints;

    if(isoPointsFlag){
        // copy vram to ram
        clQueue->enqueueReadBuffer(*clIsoPoints ,CL_TRUE,0,sizeof(IsoPoint) * NUM_ISO_POINTS, isoPoints, NULL, &event);
        event.wait();
        //ofLog() << "number of valid cubes: " << inspector.numberOfValidCubes << " number of valid indicies: " << numValidIndicies;
        // isoValue to color
        for (int i = 0; i < NUM_ISO_POINTS; i++) {
            isoPointsColors[i].r = isoPoints[i].isoValue * 1000.0;
        }
        isoPointsVBO.updateColorData(isoPointsColors, NUM_ISO_POINTS);
    }
}

void Metaball::draw(){

    if(metaballFlag){
        glEnable(GL_DEPTH_TEST);
        shader.begin();
            shader.setUniform4fv("LightPosition", lightPosition.getPtr());
            shader.setUniform3f("Kd", 1.0, 1.0, 1.0);
            shader.setUniform3f("Ld", 1.0, 1.0, 1.0);

            shader.setUniformMatrix4f("ModelViewMatrix",modelViewMatrix);
            shader.setUniformMatrix4f("ProjectionMatrix",projectionMatrix);
        
            // currently not implement in oF
            GLuint prog = shader.getProgram();
            int loc = glGetUniformLocation(prog, "NormalMatrix");
            if(loc < 0){
                ofLog() << "no such uniform, NormalMatrix";
            }else{
                float rawMatrix[9] =
                {   normalMatrix.a, normalMatrix.b, normalMatrix.c,
                    normalMatrix.d, normalMatrix.e, normalMatrix.f,
                    normalMatrix.g, normalMatrix.h, normalMatrix.i};
                glUniformMatrix3fv(loc, 1, GL_FALSE, rawMatrix);
            }
            // end dirty code
        
        
            shader.setUniformMatrix4f("MVP", MVP);
        
            triangleSurfaceVBO.draw(GL_TRIANGLE_STRIP, 0, numValidPoints);

        shader.end();
        glDisable(GL_DEPTH_TEST);
    }
    
    if(isoPointsFlag){
        isoPointsVBO.draw(GL_POINTS, 0, NUM_ISO_POINTS);
    }
    
}

void Metaball::toggleIsoPointsFlag(){
    isoPointsFlag = !isoPointsFlag;
}

void Metaball::toggleMetaballFlag(){
    metaballFlag = !metaballFlag;
}



