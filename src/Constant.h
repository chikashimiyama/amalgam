//
//  Constant.h
//  amalgam
//
//  Created by Chikashi Miyama on 19/07/14.
//
//

#ifndef amalgam_Constant_h
#define amalgam_Constant_h

#define NUM_PARTICLES 16
#define BOX_SIZE 410
#define RESOLUTION 41
#define OFFSET 20
#define RESOLUTION_MINUS_ONE (RESOLUTION - 1)

#define STEP (BOX_SIZE / RESOLUTION)
#define NUM_ISO_POINTS (RESOLUTION * RESOLUTION * RESOLUTION)
#define NUM_ISOCUBES (RES_MINUS_ONE * RES_MINUS_ONE * RES_MINUS_ONE)
#define ISO_VALUE 0.000563
#endif
