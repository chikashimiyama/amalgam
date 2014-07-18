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
    cl_float position[3];
    cl_float orientation[3];
    cl_float acceleration[3];
    cl_int visible;
    cl_int age;
    cl_int dummy[5];
};

#endif /* defined(__amalgam__Points__) */
