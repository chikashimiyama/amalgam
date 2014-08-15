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
    speedLimitP.set("speed limit", 5 , 5, 50);

    emitterPG.setName("Emitter");
    emitterPG.add(originPG);
    emitterPG.add(orientationPG);
    emitterPG.add(accelerationPG);
    emitterPG.add(numSpawnP);
    emitterPG.add(isoAttenuationP);
    emitterPG.add(speedLimitP);
    
    // force parameter group
    turbulenceP.set("turbulance", 0, 0, 30);
    gravitationP.set("gravitation", 0.5, 0, 2.0);

    forcePG.setName("Force");
    forcePG.add(turbulenceP);
    forcePG.add(gravitationP);
    
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
        particles[i].orientation += particles[i].acceleration;
        particlePosition[i] += turbulenceP.get() * ofVec3f(ofRandom(-1.0, 1.0), ofRandom(-1.0, 1.0), ofRandom(-1.0, 1.0));
        float dist = particlePosition[i].distance(ofVec3f());
        
        //if very close to the center, fire it.
        if (dist < 20.0) {
            particlePosition[i] = ofVec3f(ofRandom(-30, 30), ofRandom(-30, 30), ofRandom(-30, 30));
            particles[i].orientation = ofVec3f(ofRandom(-10, 10), ofRandom(-10, 10), ofRandom(-10, 10));
            particles[i].acceleration = ofVec3f(0.0, 0.0, 0.0);

        }
        particles[i].acceleration = -1.0 * (particlePosition[i] - ofVec3f(0.0, 0.0, 0.0)) * gravitationP.get() * 0.01;
        float limit = speedLimitP.get();
        particles[i].orientation.x = ofClamp(particles[i].orientation.x, -limit, limit);
        particles[i].orientation.y = ofClamp(particles[i].orientation.y, -limit, limit);
        particles[i].orientation.z = ofClamp(particles[i].orientation.z, -limit, limit);

    }

}

void Emitter::update(void){
    updateFromParameters();
    updateParticlePosition();
    particleVbo.setVertexData(particlePosition, NUM_PARTICLES, GL_DYNAMIC_DRAW);
}

void Emitter::updateFromParameters(){
    
}

void Emitter::draw(void){
    if(drawFlag){
        glPointSize(5);
        particleVbo.draw(GL_POINTS, 0, NUM_PARTICLES);
    }
}
