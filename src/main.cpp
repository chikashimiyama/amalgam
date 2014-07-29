#include "ofMain.h"
#include "ofApp.h"
#include "ofGLProgrammableRenderer.h"

//========================================================================
int main( ){
    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);

	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context
	ofRunApp(new ofApp());

}
