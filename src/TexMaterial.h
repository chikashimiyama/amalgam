//
//  TexMaterial.h
//  amalgam
//
//  Created by Chikashi Miyama on 24/07/14.
//
//

#ifndef __amalgam__TexMaterial__
#define __amalgam__TexMaterial__

#include "ofMain.h"

class TexMaterial{
    
    ofFbo textureFbo;
public:
    void setup();
    void customDraw();
};

#endif /* defined(__amalgam__TexMaterial__) */
