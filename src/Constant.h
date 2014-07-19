//
//  Constant.h
//  amalgam
//
//  Created by Chikashi Miyama on 19/07/14.
//
//

#ifndef amalgam_Constant_h
#define amalgam_Constant_h

#define NUM_PARTICLES 64
#define BOX_SIZE 400
#define RESOLUTION 40
#define RES_MINUS_ONE (RESOLUTION - 1)

#define STEP (BOX_SIZE / RESOLUTION)
#define NUM_ISO_POINTS (RESOLUTION * RESOLUTION * RESOLUTION)
#define NUM_ISOCUBES (RES_MINUS_ONE * RES_MINUS_ONE * RES_MINUS_ONE)
#define ISO_VALUE 0.000563
#endif
