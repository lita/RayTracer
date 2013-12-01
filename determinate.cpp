/*
 *  determinate.cpp
 *  RayTracer
 *
 *  Created by Lita Cho on 9/28/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "algebra3.h"


//////////////////////////////////////////////////////////////////////////////
//                            MAIN FUNCTION                                 //
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    mat3 test1 = mat3(vec3(0,1,0), vec3(1,0,0),vec3(0,0,1));
    printf("test1: %f", test1.determinant());
}
