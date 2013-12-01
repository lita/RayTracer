#include "Camera.h"
#include <cfloat>
#include <time.h>


/* Constructors */
Camera::Camera(){}

Camera::Camera(const vec3& ul, const vec3& ll,
			   const vec3& ur, const vec3& lr) {

	viewplaneUL = ul;
	viewplaneLL = ll;
	viewplaneUR = ur;
	viewplaneLR = lr;

}

/* Destructior */
Camera::~Camera() {}


//////////////////////////
// Pinhole Camera       //
//////////////////////////

/* Constructors */
PinholeCamera::PinholeCamera(const vec3& viewPos, const vec3& ul, const vec3& ll, const vec3& ur, const vec3& lr):
    Camera(ul, ll, ur, lr) {
    this->viewerPos = viewPos;
}


/* Instance methods */

// Requires that normSamp be normalized.
Ray PinholeCamera::createViewingRay(const Sample& normSamp) {

	double u = normSamp.horiz;
	double v = normSamp.vert;

	//vec3 leftVertInterp = v*viewplaneLL + (1-v)*viewplaneUL;
	//vec3 rightVertInterp = v*viewplaneLR + (1-v)*viewplaneUR;
	//vec3 rayEnd = u*leftVertInterp + (1-u)*rightVertInterp;
	vec3 leftVertInterp = v*viewplaneUL + (1-v)*viewplaneLL;
	vec3 rightVertInterp = v*viewplaneUR + (1-v)*viewplaneLR;
	vec3 rayEnd = u*rightVertInterp + (1-u)*leftVertInterp;

	return Ray(viewerPos, rayEnd, 1, DBL_MAX, normSamp, NULL);
}

//////////////////////////
// Lens Camera          //
//////////////////////////

/* Constructors */
LensCamera::LensCamera(const vec3& postion, const vec3& edge1, const vec3& edge2,
                       const vec3& ul, const vec3& ll, const vec3& ur, const vec3& lr, 
                       int sampleRate):
    Camera(ul, ll, ur, lr) {
    this->position = postion;
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

Ray LensCamera::createViewingRay(const Sample& normSamp) {
    double u = normSamp.horiz;
    double v = normSamp.vert;
    
    vec3 leftVertInterp = v*viewplaneUL + (1-v)*viewplaneLL;
	vec3 rightVertInterp = v*viewplaneUR + (1-v)*viewplaneLR;
	vec3 rayEnd = u*rightVertInterp + (1-u)*leftVertInterp;

    int p = normSamp.p;
    int q = normSamp.q;
    vec3 viewerPos = jitteredPoints[p][q];
    return Ray(viewerPos, rayEnd, 1, DBL_MAX, normSamp, NULL);
}

