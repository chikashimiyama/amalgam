//
//  Emitter.cpp
//  amalgam
//
//  Created by Chikashi Miyama on 19/07/14.
//
//

#include "Emitter.h"

Emitter::~Emitter(){
    
    if(clParticleBuffer){
        delete clParticleBuffer;
        ofLog() << "clParticleBuffer deleted";
    }
    if(clParticleSettingBuffer){
        delete clParticleSettingBuffer;
        ofLog() << "clParticleSettingBuffer deleted";
    }
    if(clBufferGL){
        delete clBufferGL;
        ofLog() << "clBufferGL deleted";
    }
    if(clKernel){
        delete clKernel;
        ofLog() << "clKernel deleted";
    }
    
    if(clUpdateKernelFunctor){
        delete clUpdateKernelFunctor;
        ofLog() << "clUpdateKernelFunctor deleted";
    }
}

void Emitter::createRandomTable(){
    for (int i = 0; i < NUM_PARTICLES * 3; i++) {
        randomTable[i] = ofRandom(-1.0, 1.0);
    }
}

void Emitter::initParticleSetting(){
    particleSetting.origin[0] = 0.0;
    particleSetting.origin[1] = 0.0;
    particleSetting.origin[2] = 0.0;
    particleSetting.originSpread[0] = 0.0;
    particleSetting.originSpread[1] = 0.0;
    particleSetting.originSpread[2] = 0.0;
    particleSetting.orientation[0] = 0.0;
    particleSetting.orientation[1] = 0.05;
    particleSetting.orientation[2] = 0.0;
    particleSetting.orientationSpread[0] = 0.1;
    particleSetting.orientationSpread[1] = 0.1;
    particleSetting.orientationSpread[2] = 0.1;
    
    particleSetting.acceleration[0] = 0.0;
    particleSetting.acceleration[1] = 0.0;
    particleSetting.acceleration[2] = 0.0;
    particleSetting.accelerationSpread[0] = 0.0;
    particleSetting.accelerationSpread[1] = 0.0;
    particleSetting.accelerationSpread[2] = 0.0;
    
    particleSetting.lifeSpan[0] = 10000;
    particleSetting.lifeSpan[1] = 10000;
    particleSetting.numberOfSpawn = 5;
    particleSetting.spawninIndex = 0;
}

void Emitter::createParameterGroups(){
    originPG.setName("origin");
    originPG.add(originP.set("origin", ofVec3f(0, 0, 0) ,ofVec3f(-10, -10, -10), ofVec3f(10, 10, 10)));
    originPG.add(originSpreadP.set("originSpread", ofVec3f(1, 1, 1) ,ofVec3f(-10, -10, -10), ofVec3f(10, 10, 10)));
    orientationPG.setName("orientation");
    orientationPG.add(orientationP.set("orientation", ofVec3f(0.0, 0.0, 0.0) ,ofVec3f(-1, -1, -1), ofVec3f(1, 1, 1)));
    orientationPG.add(orientationSpreadP.set("orientationSpread",ofVec3f(0.1, 0.1, 0.1) ,ofVec3f(0, 0, 0), ofVec3f(1, 1, 1)));
    accelerationPG.setName("acceleration");
    accelerationPG.add(accelerationP.set("acceleration",ofVec3f(0, 0, 0) ,ofVec3f(-0.01, -0.01, -0.01), ofVec3f(0.01, 0.01, 0.01)));
    accelerationPG.add(accelerationSpreadP.set("accelerationSpread", ofVec3f(0, 0, 0) ,ofVec3f(-0.01, -0.01, -0.01), ofVec3f(0.01, 0.01, 0.01)));
    numSpawnP.set("numSpawn", 5, 0 ,10);
    emitterPG.setName("Emitter");
    emitterPG.add(originPG);
    emitterPG.add(orientationPG);
    emitterPG.add(accelerationPG);
}

void Emitter::setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue){
    Emitter::clQueue = clQueue;
    clParticleBuffer = new cl::Buffer(*clContext,CL_MEM_READ_WRITE,sizeof(Particle)*NUM_PARTICLES);
    clQueue->enqueueWriteBuffer(
                                *clParticleBuffer ,CL_TRUE,0,
                                sizeof(Particle)*NUM_PARTICLES, particleBuffer);

    clParticleSettingBuffer = new cl::Buffer(*clContext,CL_MEM_READ_WRITE,sizeof(particleSetting));
    clQueue->enqueueWriteBuffer(
                                *clParticleSettingBuffer ,CL_TRUE,0,
                                sizeof(ParticleSetting), &particleSetting);

    clKernel = new cl::Kernel(*clProgram, "update");
    clUpdateKernelFunctor = new cl::KernelFunctor(*clKernel, *clQueue, cl::NullRange, cl::NDRange(NUM_PARTICLES), cl::NullRange);
    
    createParameterGroups();

    dotsVBO.setVertexData(dots, NUM_PARTICLES, GL_DYNAMIC_DRAW);
    int vboId = dotsVBO.getVertId();
    clBufferGL = new cl::BufferGL(*clContext, CL_MEM_READ_WRITE, vboId);
    
    ofLog() << "create random Table";
    clRandomTable = new cl::Buffer(*clContext,CL_MEM_READ_WRITE,sizeof(float) * NUM_PARTICLES * 3);
    createRandomTable();
    clQueue->enqueueWriteBuffer(
                                *clRandomTable ,CL_TRUE,0,
                                sizeof(float) * NUM_PARTICLES * 3 , randomTable);
    
    
}

void Emitter::update(void){
    updateFromParameters();
    clQueue->enqueueWriteBuffer(*clParticleSettingBuffer ,CL_TRUE,0,sizeof(ParticleSetting), &particleSetting);
    
    // let GPU to calculate the movement of particle
    (*clUpdateKernelFunctor)(*clParticleBuffer, *clParticleSettingBuffer, *clBufferGL, *clRandomTable);
    
    particleSetting.spawninIndex += particleSetting.numberOfSpawn;
    if (particleSetting.spawninIndex > NUM_PARTICLES) {
        particleSetting.spawninIndex %= NUM_PARTICLES;
    }
}

void Emitter::updateFromParameters(){
    
        particleSetting.origin[0] = originP.get().x;
        particleSetting.origin[1] = originP.get().y;
        particleSetting.origin[2] = originP.get().z;
        particleSetting.originSpread[0] = originSpreadP.get().x;
        particleSetting.originSpread[1] = originSpreadP.get().y;
        particleSetting.originSpread[2] = originSpreadP.get().z;
    
        particleSetting.orientation[0] = orientationP.get().x;
        particleSetting.orientation[1] = orientationP.get().y;
        particleSetting.orientation[2] = orientationP.get().z;
        particleSetting.orientationSpread[0] = orientationSpreadP.get().x;
        particleSetting.orientationSpread[1] = orientationSpreadP.get().y;
        particleSetting.orientationSpread[2] = orientationSpreadP.get().z;
    
        particleSetting.acceleration[0] = accelerationP.get().x;
        particleSetting.acceleration[1] = accelerationP.get().y;
        particleSetting.acceleration[2] = accelerationP.get().z;
        particleSetting.accelerationSpread[0] = accelerationSpreadP.get().x;
        particleSetting.accelerationSpread[1] = accelerationSpreadP.get().y;
        particleSetting.accelerationSpread[2] = accelerationSpreadP.get().z;
    
        particleSetting.numberOfSpawn = numSpawnP.get();
}

void Emitter::draw(void){
    dotsVBO.draw(GL_POINTS, 0, NUM_PARTICLES);

}
