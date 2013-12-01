#include "rgb.h"


/* Special functions */
rgb& rgb::normalize() {

	n[VX] = MIN(1.0, n[VX]);
	n[VY] = MIN(1.0, n[VY]);
	n[VZ] = MIN(1.0, n[VZ]);

	n[VX] = MAX(0.0, n[VX]);
	n[VY] = MAX(0.0, n[VY]);
	n[VZ] = MAX(0.0, n[VZ]);

	return *this;

}

const rgb rgb::black = rgb(0,0,0);