#ifndef RAYH
#define RAYH

// Forward declarations
class Primitive;

#include "algebra3.h"
#include "Sampler.h"


/* Represents a viewing/shadow/reflection/refraction
   ray. */
class Ray {

private:

	/* Instance vars */
	vec3 origin;					// Point of origin
	vec3 direction;					// Direction ray points.
    vec3 inverseDirection;          // Direction of the inverse
	double tMin;					// Minimum intersection bound.
	double tMax;					// Maximum intersection bound.
    int sign[3];                    // holds the sign of all the vectors
	Sample mySample;				// The sample that generated THIS ray.
    Primitive* lastHit;             // Stores in the last primitive hit.

public:

	/* Constructors */
	Ray(const vec3& start, const vec3& end, double min, double max, const Sample& samp, Primitive* lastHit);
	Ray(const vec3& start, double min, double max, const vec3& direction, const Sample& samp, Primitive* lastHit);

	/* Instance methods */
	inline vec3 getOrigin() { return origin; }
	inline vec3 getDirection() { return direction; }
    inline vec3 getInverseDirection() {return inverseDirection; }
    inline int getSign(int axis) { return sign[axis % 3]; }
	inline Sample getSample() { return mySample; }
	inline double getLowerBound() { return tMin; }
	inline double getUpperBound() { return tMax; }
	inline bool isWithinBounds(double tVal) { return tVal <= tMax && tVal >= tMin; }
	inline void setBounds(double min, double max) { 
		tMin = min;
		tMax = max;
	}
    inline Primitive* getLastHitPrim () { return lastHit; }
	inline vec3 intersectionPoint(double t) { return origin + t * direction; }
	inline Ray& operator = (const Ray& ray);

	/* Friends */
	friend Ray operator * (const mat4& transform, Ray& ray);

};


inline Ray& Ray::operator = (const Ray& ray) {
	origin = ray.origin;
	direction = ray.direction;
	tMin = ray.tMin;
	tMax = ray.tMax;
    inverseDirection = ray.inverseDirection;
    sign[0] = (inverseDirection[0] < 0);
    sign[1] = (inverseDirection[1] < 0);
    sign[2] = (inverseDirection[2] < 0);
	mySample = ray.mySample;
    lastHit = ray.lastHit;
	return *this;
}

inline Ray operator * (const mat4& transform, Ray& ray) {

	Ray returnRay = ray;
	returnRay.origin = vec3(transform * vec4(ray.origin, 1.0));
	returnRay.direction = vec3(transform * vec4(ray.direction, 0.0), VW);
    returnRay.inverseDirection = vec3(1/returnRay.direction[0], 1/returnRay.direction[1], 1/returnRay.direction[2]);
    returnRay.sign[0] = (returnRay.inverseDirection[0] < 0);
    returnRay.sign[1] = (returnRay.inverseDirection[1] < 0);
    returnRay.sign[2] = (returnRay.inverseDirection[2] < 0);
    returnRay.lastHit = ray.lastHit;
	return returnRay;
}

#endif