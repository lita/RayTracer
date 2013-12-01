#include "RayTracer.h"
#include "Material.h"
#include "Lights.h"
#include <cfloat>
#include <cmath>
#include <vector>
#include <cmath>

using namespace std;


/* Constructors */

RayTracer::RayTracer(Scene* scene, unsigned int depth, double bias) {

	tracingScene = scene;
	recursionDepth = depth;
	rayBias = bias;

}


/* Instance methods */

// Publicly accessible
rgb RayTracer::traceViewingRay(Ray& ray) {

	return trace(ray, recursionDepth);
}

// Recursive helper for the above method.
rgb RayTracer::trace(Ray& ray, unsigned int depth) {

	// Recursive base case
	if (depth < 0)
		return rgb::black;

	IntersectRecord rec;
	if (tracingScene->getHierarchy()->intersect(ray, &rec))
		return shadeIntersection(rec, ray, depth);
	else return rgb::black;

}

rgb RayTracer::shadeIntersection(const IntersectRecord& intersection, Ray& ray, unsigned int depth) {
	
	Reflectance refl = intersection.primitive->getReflectance(intersection.point);
	rgb pointColor = refl.kA;
	vector<Light*> lights = tracingScene->getLights();

	for (unsigned int i = 0; i < lights.size(); i++) {
		Ray shadowRay = lights[i]->getShadowRay(intersection.point, rayBias, ray);
        rgb kT = intersection.primitive->getReflectance(intersection.point).kT;
		if (!traceShadowRay(shadowRay)) {
			vec3 lightIncidence = shadowRay.getDirection();
			lightIncidence.normalize();
			if (refl.kD != rgb::black)
				pointColor += diffComp(intersection, lightIncidence, lights[i]->getColor());
			if (refl.kS != rgb::black)
				pointColor += specComp(intersection, lightIncidence, lights[i]->getColor(), ray);
		}
	}
    
	// Bounce rays
    
	if (refl.kR != rgb::black && depth > 0) {
        vec3 rayDirection = ray.getDirection();
        vec3 reflectDirection = rayDirection -
        2*(rayDirection * intersection.surfaceNormal) * intersection.surfaceNormal;
		Ray bounceRay(intersection.point, rayBias, DBL_MAX, reflectDirection, ray.getSample(), ray.getLastHitPrim());
		pointColor += refl.kR * trace(bounceRay, depth - 1);
	}
    
    // Refraction Rays
    if (refl.kT != rgb(0,0,0) && depth > 0) {
		double index = intersection.primitive->getReflectance(intersection.point).indexOfRefraction;
		bool refracted = false;
        vec3 refractDirection (0,0,0);
        if (ray.getLastHitPrim() != NULL) {          
            if (ray.getLastHitPrim() == intersection.primitive) {
                    refracted = refract(ray, intersection, index, 1.0, refractDirection);
            } else {
                double oldIndex = ray.getLastHitPrim()->getReflectance(intersection.point).indexOfRefraction;
                refracted = refract(ray, intersection, oldIndex, index, refractDirection);
            }
        } else {
            refracted = refract(ray, intersection, 1.0, index, refractDirection);
        }
        
        if (refracted) {
            Ray refractRay(intersection.point, rayBias, DBL_MAX, refractDirection, ray.getSample(), intersection.primitive);
            pointColor += refl.kT * trace(refractRay, depth - 1);
        }
    }
	return pointColor;
}

bool RayTracer::traceShadowRay(Ray& ray) {

	IntersectRecord rec;
	// If we hit the last primitive through which the ray was transmitted via
	// refraction, don't register that as an occluder.
	return (tracingScene->getHierarchy()->intersect(ray, &rec) &&
		rec.primitive != ray.getLastHitPrim());
}

rgb RayTracer::diffComp(const IntersectRecord& intersection, const vec3& incidence, const rgb& color) {

	return intersection.primitive->getReflectance(intersection.point).kD * color
		* MAX(intersection.surfaceNormal * incidence, 0);

}

rgb RayTracer::specComp(const IntersectRecord& intersection, const vec3& incidence, const rgb& color, Ray& viewRay) {

	vec3 reflectVec = -incidence + (2 * (incidence * intersection.surfaceNormal) * intersection.surfaceNormal);
	vec3 viewerVec = -viewRay.getDirection();
	viewerVec.normalize();
	double scalarTerm = MAX(reflectVec * viewerVec, 0);
	Reflectance reflec = intersection.primitive->getReflectance(intersection.point);
	return reflec.kS * color * pow(scalarTerm, reflec.pExp);
}

bool RayTracer::refract(Ray& ray, const IntersectRecord& intersect, double oldIndex, double newIndex, vec3& refractDirection) {
    double n = oldIndex/newIndex;
    vec3 direction = ray.getDirection();
	direction.normalize();
    vec3 normal = intersect.surfaceNormal;

    // Checking whether image is going into the object or going out. If greater than 90 degrees, 
    // then it is going into the object. If less, then going out.
    double cosAngle = direction * normal.normalize();
    if(cosAngle  > 0) {
        normal = -intersect.surfaceNormal;
    }
    
    double c = direction * normal;
    double cosPhi2 = (1 - ((n * n) * (1 - (c * c))));
    
    // If cos(phi)^2 is less than 0, then no refraction ray exists and all 
    // the energy is reflected. 
    if (cosPhi2 < 0) 
        return false;
    else {
        double cosPhi = sqrt(cosPhi2);
        vec3 term1 = n * (direction - normal * (c));
        refractDirection = term1 - normal * cosPhi;
        
        /*
        vec3 term1 = n * c - cosPhi;
        refractDirection = direction * n + normal * term1;
        */
        return true;
    }
}