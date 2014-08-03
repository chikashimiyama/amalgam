//
//  Particle.h
//  amalgam
//
//  Created by Chikashi Miyama on 17/07/14.
//
//

#ifndef __amalgam__Particle__
#define __amalgam__Particle__

#include <cl.hpp>



struct ParticleSetting{
    cl_float origin[3];
    cl_float originSpread[3];
    cl_float orientation[3];
    cl_float orientationSpread[3];
    cl_float acceleration[3];
    cl_float accelerationSpread[3];
    cl_float turbulence[3];
    cl_uint lifeSpan[2];
    cl_uint numberOfSpawn;
    cl_uint spawninIndex;
    cl_int dummy[7];
};

typedef struct IsoPoint{
    cl_float x;
    cl_float y;
    cl_float z;
    cl_float xn;
    cl_float yn;
    cl_float zn;
    cl_float isoValue;
    cl_float dummy;

} IsoPoint;

typedef struct Inspector{
    cl_float isoThreshold;
    cl_int numberOfValidCubes;
    cl_int numberOfValidPoints;
    cl_int dummy;
} Inspector;

#endif /* defined(__amalgam__Particle__) */
