//
//  Emitter.cpp
//  amalgam
//
//  Created by Chikashi Miyama on 19/07/14.
//
//

#include "Emitter.h"

Emitter::~Emitter(){
  
    if(clParticleBufferGL){
        delete clParticleBufferGL;
        ofLog() << "clParticle deleted";
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
    
    forcePG.setName("Force");
    forcePG.add(turbulenceP);
}

void Emitter::setup(cl::Context *clContext, cl::Program *clProgram, cl::CommandQueue *clQueue){
    
    drawFlag = false;
    createParameterGroups();

    particleVbo.setVertexData(particlePosition, NUM_PARTICLES, GL_DYNAMIC_DRAW);
    clParticleBufferGL =new  cl::BufferGL(*clContext, CL_MEM_READ_WRITE, particleVbo.getVertId());
    
    ofLog() << "create random Table";
    clRandomTable = new cl::Buffer(*clContext,CL_MEM_WRITE_ONLY,sizeof(float) * SIZE_OF_RANDOM_TABLE);
    createRandomTable();
    clQueue->enqueueWriteBuffer(*clRandomTable ,CL_TRUE,0,sizeof(float) * SIZE_OF_RANDOM_TABLE, randomTable);
}


void Emitter::updateParticlePosition(){


    for (int i = 0; i < NUM_PARTICLES; i++) {
        
        particlePosition[i] += particles[i].orientation;
        particlePosition[i] += turbulenceP.get() * ofVec3f(ofRandom(1.0), ofRandom(1.0), ofRandom(1.0));
        particles[i].orientation += particles[i].acceleration;
        float dist = particlePosition[i].distance(ofVec3f());
        if (dist < 50.0) {
            particles[i].orientation = ofVec3f(ofRandom(-10, 10), ofRandom(-10, 10), ofRandom(-10, 10));
            particles[i].acceleration = ofVec3f(0.0, 0.0, 0.0);

        }
        particles[i].acceleration = (particlePosition[i] - ofVec3f(0.0, 0.0, 0.0)).normalize() * -0.5;
    }

}

void Emitter::update(void){
    updateFromParameters();
    updateParticlePosition();
    particleVbo.setVertexData(particlePosition, NUM_PARTICLES, GL_DYNAMIC_DRAW);
}

void Emitter::updateFromParameters(){

//    particleSetting.origin[0] = originP.get().x;
//    particleSetting.origin[1] = originP.get().y;
//    particleSetting.origin[2] = originP.get().z;
//    particleSetting.originSpread[0] = originSpreadP.get().x;
//    particleSetting.originSpread[1] = originSpreadP.get().y;
//    particleSetting.originSpread[2] = originSpreadP.get().z;
//
//    particleSetting.orientation[0] = orientationP.get().x;
//    particleSetting.orientation[1] = orientationP.get().y;
//    particleSetting.orientation[2] = orientationP.get().z;
//    particleSetting.orientationSpread[0] = orientationSpreadP.get().x;
//    particleSetting.orientationSpread[1] = orientationSpreadP.get().y;
//    particleSetting.orientationSpread[2] = orientationSpreadP.get().z;
//
//    particleSetting.acceleration[0] = accelerationP.get().x;
//    particleSetting.acceleration[1] = accelerationP.get().y;
//    particleSetting.acceleration[2] = accelerationP.get().z;
//    particleSetting.accelerationSpread[0] = accelerationSpreadP.get().x;
//    particleSetting.accelerationSpread[1] = accelerationSpreadP.get().y;
//    particleSetting.accelerationSpread[2] = accelerationSpreadP.get().z;
//
//    particleSetting.turbulence[0] = turbulenceP.get().x;
//    particleSetting.turbulence[1] = turbulenceP.get().y;
//    particleSetting.turbulence[2] = turbulenceP.get().z;
//
//    particleSetting.numberOfSpawn = numSpawnP.get();
}

void Emitter::draw(void){
    if(drawFlag){
        glPointSize(5);
        particleVbo.draw(GL_POINTS, 0, NUM_PARTICLES);
    }
}
