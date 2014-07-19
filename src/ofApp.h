#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "cl.hpp"
#include "ofxGui.h"
#include "Emitter.h"
#include "Constant.h"



class ofApp : public ofBaseApp{
    private:
        void initParticleSetting();
        void createRandomTable();
        void setupGui();
        // GL
        ofCamera camera;

        // OpenCl
        cl::Program *clProgram;
        cl::Context *clContext;
        cl::CommandQueue *clQueue;
    
        // gui
        Emitter emitter;
        ofxPanel panel;

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
