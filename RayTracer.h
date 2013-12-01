#ifndef RAYTRACERH
#define RAYTRACERH

#include "algebra3.h"
#include "Scene.h"
#include "Ray.h"
#include "Primitives.h"
#include "IntersectRecord.h"

/* Raytracer objects trace viewing rays into the
   scene in order to determine sample color. */
class RayTracer {

private:

	/* Instance vars */
	Scene* tracingScene;						// Scene into which to trace rays
	unsigned int recursionDepth;				// Maximum # of ray bounces
	double rayBias;							// tMin for shadow/bounce rays
    double shadowBias;

	/* Instance methods */
	rgb trace(Ray& ray, unsigned int depth);
	bool traceShadowRay(Ray& ray);
	rgb shadeIntersection(const IntersectRecord& intersection, Ray& ray, unsigned int depth);
	rgb diffComp(const IntersectRecord& intersection, const vec3& incidence, const rgb& color);
	rgb specComp(const IntersectRecord& intersection, const vec3& incidence, const rgb& color, Ray& viewRay);
    bool refract(Ray& ray, const IntersectRecord& intersect, double oldIndex, double newIndex, vec3& refractDirection);

public:

	/* Constructors */
	RayTracer(Scene* scene, unsigned int depth, double bias);

	/* Instance methods */
	rgb traceViewingRay(Ray& ray);

};


#endif