//
//  Constant.h
//  amalgam
//
//  Created by Chikashi Miyama on 19/07/14.
//
//

#ifndef amalgam_Constant_h
#define amalgam_Constant_h

#define SIZE_OF_RANDOM_TABLE 8192
#define NUM_PARTICLES 16
#define BOX_SIZE 860
#define RESOLUTION 40
#define OFFSET 20
#define RESOLUTION_MINUS_ONE (RESOLUTION - 1)

#define NUMBER_OF_COMP_UNITS 6
#define LOCAL_RESOLUTION RESOLUTION_MINUS_ONE/NUMBER_OF_COMP_UNITS

#define STEP (BOX_SIZE / RESOLUTION)
#define NUM_ISO_POINTS (RESOLUTION * RESOLUTION * RESOLUTION)
#define NUM_ISOCUBES (RESOLUTION_MINUS_ONE * RESOLUTION_MINUS_ONE * RESOLUTION_MINUS_ONE)
#endif
