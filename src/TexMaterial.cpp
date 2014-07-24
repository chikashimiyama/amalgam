//
//  TexMaterial.cpp
//  amalgam
//
//  Created by Chikashi Miyama on 24/07/14.
//
//

#include "TexMaterial.h"


void TexMaterial::setup(){
    // fbo setup
    textureFbo.allocate(640, 480);

    textureFbo.begin();
    ofClear(0, 0, 0);
    textureFbo.end();
    
    textureFbo.begin();
    ofBackground(0, 0, 0);
    ofSetColor(255,255,255, 1.0);
    ofRect(10,10,400,400);
    textureFbo.end();
}

void TexMaterial::customDraw(){
    
    textureFbo.draw(0, 0);
}