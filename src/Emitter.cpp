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
    if(clParticleBufferGL){
        delete clParticleBufferGL;
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
    for (int i = 0; i < SIZE_OF_RANDOM_TABLE; i++) {
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

    particleSetting.turbulence[0] = 0.0;
    particleSetting.turbulence[1] = 0.0;
    particleSetting.turbulence[2] = 0.0;

    
    particleSetting.lifeSpan[0] = 10000;
    particleSetting.lifeSpan[1] = 10000;
    particleSetting.numberOfSpawn = 1;
    particleSetting.spawninIndex = 0;
    
    
}

void Emitter::createParameterGroups(){
    
    // emitter parameter group
    originPG.setName("origin");
    originPG.add(originP.set("origin", ofVec3f(0, 0, 0) ,ofVec3f(-400, -400, -400), ofVec3f(400, 400, 400)));
    originPG.add(originSpreadP.set("originSpread", ofVec3f(1, 1, 1) ,ofVec3f(-300, -300, -300), ofVec3f(300, 300, 300)));
    
    orientationPG.setName("orientation");
    orientationPG.add(orientationP.set("orientation", ofVec3f(0.0, 0.0, 0.0) ,ofVec3f(-10, -10, -10), ofVec3f(10, 10, 10)));
    orientationPG.add(orientationSpreadP.set("orientationSpread",ofVec3f(0.1, 0.1, 0.1) ,ofVec3f(0, 0, 0), ofVec3f(10, 10, 10)));
    accelerationPG.setName("acceleration");
    accelerationPG.add(accelerationP.set("acceleration",ofVec3f(0, 0, 0) ,ofVec3f(-1, -1, -1), ofVec3f(1, 1, 1)));
    accelerationPG.add(accelerationSpreadP.set("accelerationSpread", ofVec3f(0, 0, 0) ,ofVec3f(-1, -1, -1), ofVec3f(1, 1, 1)));
    numSpawnP.set("numSpawn", 1, 0 ,10);
    isoAttenuationP.set("iso attenuation", 1.0, 0.0, 2.0);
    
    emitterPG.setName("Emitter");
    emitterPG.add(originPG);
    emitterPG.add(orientationPG);
    emitterPG.add(accelerationPG);
    emitterPG.add(numSpawnP);
    emitterPG.add(isoAttenuationP);
    
    // force parameter group
    turbulenceP.set("turbulance", ofVec3f(0, 0, 0), ofVec3f(0, 0,  0), ofVec3f(100,100,100));
    tornadeP.set("tornade", ofVec3f(0, 0, 0), ofVec3f(-20, -20, -20), ofVec3f(20,20,20));

    
    forcePG.setName("Force");
    forcePG.add(turbulenceP);
    forcePG.add(tornadeP);
    
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
    clParticleBufferGL = new cl::BufferGL(*clContext, CL_MEM_READ_WRITE, vboId);
    
    ofLog() << "create random Table";
    clRandomTable = new cl::Buffer(*clContext,CL_MEM_WRITE_ONLY,sizeof(float) * SIZE_OF_RANDOM_TABLE);
    createRandomTable();
    clQueue->enqueueWriteBuffer(*clRandomTable ,CL_TRUE,0,sizeof(float) * SIZE_OF_RANDOM_TABLE, randomTable);
}

void Emitter::update(void){
    updateFromParameters();
    int randomSeed = static_cast<int>(ofRandom(SIZE_OF_RANDOM_TABLE));
    cl::Event event;
    clQueue->enqueueWriteBuffer(*clParticleSettingBuffer ,CL_TRUE,0,sizeof(ParticleSetting), &particleSetting, NULL, &event);
    (*clUpdateKernelFunctor)(*clParticleBuffer, *clParticleSettingBuffer, *clParticleBufferGL, *clRandomTable, randomSeed, NULL, &event);
    event.wait();
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
    
        particleSetting.turbulence[0] = turbulenceP.get().x;
        particleSetting.turbulence[1] = turbulenceP.get().y;
        particleSetting.turbulence[2] = turbulenceP.get().z;
  
        particleSetting.numberOfSpawn = numSpawnP.get();
}

void Emitter::draw(void){
    
    glPointSize(20);
    dotsVBO.draw(GL_POINTS, 0, NUM_PARTICLES);
    glPointSize(1);

}
