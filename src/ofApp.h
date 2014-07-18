#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "cl.hpp"
#define NUM_PARTICLES 1024

class ofApp : public ofBaseApp{
    private:
        Particle particleBuffer[NUM_PARTICLES];
        cl::Program *clProgram;
        cl::Context *clContext;
        cl::CommandQueue *clQueue;
        cl::Kernel *clKernel;
        cl::KernelFunctor *clUpdateKernelFunctor;
        cl::Buffer *clParticleBuffer;
        cl::BufferGL *clBufferGL;
        ofCamera camera;
        ofVec3f dots[NUM_PARTICLES];
        ofFloatColor dotColors[NUM_PARTICLES];
        ofVbo dotsVBO;
    
	public:
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
