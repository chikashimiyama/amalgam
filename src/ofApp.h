#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "cl.hpp"
#define NUM_PARTICLES 1024

class ofApp : public ofBaseApp{
    private:
        cl::Program *clProgram;
        cl::Context *clContext;
        cl::CommandQueue *clQueue;
        cl::Kernel *clKernel;
        cl::KernelFunctor *clUpdateKernelFunctor;
        ofCamera camera;
    
        // buffers
        cl::Buffer *clParticleBuffer; // GPU
        Particle particleBuffer[NUM_PARTICLES]; // CPU
        cl::Buffer *clParticleSettingBuffer; // GPU
        ParticleSetting particleSetting; // CPU
        cl::BufferGL *clBufferGL; // GPU -CL
        ofVbo dotsVBO; // GPU -GL
        ofVec3f dots[NUM_PARTICLES]; // CPU
    
        cl::Buffer *clRandomTable;
        float randomTable[NUM_PARTICLES * 3];
    
        void initParticleSetting();
        void createRandomTable();
	public:
        ~ofApp();
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
};
