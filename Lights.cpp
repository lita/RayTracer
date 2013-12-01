#include <cfloat>
#include "Lights.h"
#include <time.h>

// Default Constructor
Light::Light() {}

Light::~Light() {}

Light::Light(rgb color) {
    illumination = color;
}

const rgb Light::getColor() {
    return illumination;
}

PointLight::PointLight(double x, double y, double z, const rgb color):
    Light(color) {
		position = vec3(x,y,z);
	}

// Returns a normalized incidence vector.
vec3 PointLight::getIncidence(const vec3 &point) {
    //vec3 lightVector = position - origin;
    //vec3 incidence = lightVector - (point - origin);
	vec3 incidence = position - point;
    incidence.normalize();
    return incidence;
}

Ray PointLight::getShadowRay(const vec3& startPoint, double bias, Ray& viewRay) {

	return Ray(startPoint, bias, 1, getIncidence(startPoint), viewRay.getSample(), NULL);

}

DirectionalLight::DirectionalLight(double x, double y, double z, const rgb color):
    Light(color) {
		direction = vec3(x,y,z);
		direction.normalize();
}

// Returns a normalized incidence vector.
vec3 DirectionalLight::getIncidence(const vec3 &point) {
    //vec3 incidence = position - origin;
    //incidence.normalize();
    return -direction;
}

Ray DirectionalLight::getShadowRay(const vec3& startPoint, double bias, Ray& viewRay) {

	return Ray(startPoint, bias, DBL_MAX, getIncidence(startPoint), viewRay.getSample(), viewRay.getLastHitPrim());

}


AreaLight::AreaLight(const vec3& position, const vec3& edge1, const vec3& edge2, const rgb color, int sampleRate) : Light(color) {

	this->position = position;
	this->edge1 = edge1;
	this->edge2 = edge2;

	randGen = new CRandomMersenne((uint32)time(NULL));

	// Initialize point grid.
	for (int i = 0; i < sampleRate; i++) {
		jitteredPoints.push_back(vector<vec3>());
	}

	// Generate jittered points.
	double stepSize = 1.0 / sampleRate;
	vector<vec3> shuffledPoints;
	for (int i = 0; i < sampleRate; i++) {
		for (int j = 0; j < sampleRate; j++) {
			vec3 point = position + (i*stepSize + randGen->Random()*stepSize)*edge1 +
				(j*stepSize + randGen->Random()*stepSize)*edge2;
			shuffledPoints.push_back(point);
		}
	}

	// Shuffle points
	int size = (int)shuffledPoints.size();
	for (int i = size - 1; i >= 0; i--) {
		int randomIndex = randGen->IRandom(0, i);
		vec3 temp = shuffledPoints[i];
		shuffledPoints[i] = shuffledPoints[randomIndex];
		shuffledPoints[randomIndex] = temp;
	}

	// Fill point grid with shuffled points
	unsigned int shuffledIndex = 0;
	for (int i = 0; i < sampleRate; i++) {
		for (int j = 0; j < sampleRate; j++) {
			jitteredPoints[i].push_back(shuffledPoints[shuffledIndex]);
			shuffledIndex++;
		}
	}

}

AreaLight::~AreaLight() {

	delete randGen;

}

vec3 AreaLight::getIncidence(const vec3 &point, Ray& viewRay) {

	int p = viewRay.getSample().p;
	int q = viewRay.getSample().q;
	vec3 incidence = jitteredPoints[p][q] - point;
	incidence.normalize();
	return incidence;
}

Ray AreaLight::getShadowRay(const vec3& startPoint, double bias, Ray& viewRay) {

	return Ray(startPoint, bias, 1, getIncidence(startPoint, viewRay), viewRay.getSample(), viewRay.getLastHitPrim());

}