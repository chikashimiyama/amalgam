#pragma once
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#include "ofMain.h"
#include "Particle.h"
#include "cl.hpp"
#include "ofxGui.h"
#include "Emitter.h"
#include "IsoPoints.h"
#include "Metaball.h"
#include "Constant.h"


class ofApp : public ofBaseApp{
    private:
#pragma mark openCL
    cl::Program *clProgram;
    cl::Context *clContext;
    cl::CommandQueue *clQueue;

#pragma mark CoreInstance
    Emitter emitter;
    Metaball metaball;
    IsoPoints isoPoints;
    ofxPanel panel;

#pragma mark Shader
    ofShader shader;
    ofVec3f lightPosition;
    ofMatrix4x4 modelMatrix;
    ofMatrix4x4 viewMatrix;
    ofMatrix4x4 projectionMatrix;
    ofMatrix4x4 modelViewMatrix;
    ofMatrix4x4 MVP;
    ofMatrix3x3 normalMatrix;

#pragma mark Setup subfunctions
    void setupGui();
    void setupScene();
    void setupCL();
    void setupShader();
    ofMatrix3x3 reduceMatrixFrom4to3(ofMatrix4x4 mat4);

#pragma makr Flags
    bool panelFlag;
    
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
