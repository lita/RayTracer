#include "Ray.h"


/* Constructors */
Ray::Ray(const vec3& start, const vec3& end, double min, double max, const Sample& samp, Primitive* lastHit) {

	origin = start;
	direction = end - start;
	tMin = min;
	tMax = max;
    inverseDirection = vec3(1/direction[0], 1/direction[1], 1/direction[2]);
    sign[0] = (inverseDirection[0] < 0);
    sign[1] = (inverseDirection[1] < 0);
    sign[2] = (inverseDirection[2] < 0);
	mySample = samp;
    this->lastHit = lastHit;
}

Ray::Ray(const vec3& start, double min, double max, const vec3& direction, const Sample& samp, Primitive* lastHit) {

	origin = start;
	this->direction = direction;
	tMin = min;
	tMax = max;
    inverseDirection = vec3(1/direction[0], 1/direction[1], 1/direction[2]);
    sign[0] = (inverseDirection[0] < 0);
    sign[1] = (inverseDirection[1] < 0);
    sign[2] = (inverseDirection[2] < 0);
	mySample = samp;
    this->lastHit = lastHit;

}