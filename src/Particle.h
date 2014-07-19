//
//  Points.h
//  amalgam
//
//  Created by Chikashi Miyama on 17/07/14.
//
//

#ifndef __amalgam__Points__
#define __amalgam__Points__

#include <iostream>
#include <cl.hpp>
#include "ofMain.h"

struct Particle{
    cl_float orientation[3];
    cl_float acceleration[3];
    cl_int lifespan;
    cl_int age;
};

struct ParticleSetting{
    cl_float origin[3];
    cl_float originSpread[3];
    cl_float orientation[3];
    cl_float orientationSpread[3];
    cl_float acceleration[3];
    cl_float accelerationSpread[3];
    cl_uint lifeSpan[2];
    cl_uint numberOfSpawn;
    cl_uint spawninIndex;
    cl_int dummy[10];
};

struct IsoPoint{
    cl_float x;
    cl_float y;
    cl_float z;
    cl_float dummy;
};

struct IsoCube{
    struct IsoPoint points[8];
};

struct IsoSurface{
    
};



#endif /* defined(__amalgam__Points__) */
