#ifndef CAMERAH
#define CAMERAH

#include "algebra3.h"
#include "Ray.h"
#include "Sampler.h"
#include "randomc.h"
#include <vector>

/* Camera objects store information about the viewer and
   the viewing plane. They also produce viewing rays. */
class Camera {

protected:

	/* Instance vars */
	vec3 viewplaneLL;					// Lower left viewplane corner.
	vec3 viewplaneLR;					// Lower right viewplane corner.
	vec3 viewplaneUL;					// Upper left viewplane corner.	
	vec3 viewplaneUR;					// Upper right viewplane corner.

public:

	/* Constructors */
    Camera();
	Camera(const vec3& ul, const vec3& ll, const vec3& ur, const vec3& lr);

    virtual ~Camera();
    
	/* Instance methods */
	virtual Ray createViewingRay(const Sample& normSamp) = 0;
};

class PinholeCamera: public Camera {
    public:
        PinholeCamera(const vec3& viewPos, const vec3& ul, const vec3& ll, const vec3& ur, const vec3& lr);
        
        Ray createViewingRay(const Sample& normSamp);
        
    private:
        vec3 viewerPos;

};

class LensCamera: public Camera {
    public:
        LensCamera(const vec3& point, const vec3& edge1, const vec3& edge2,
                   const vec3& ul, const vec3& ll, const vec3& ur, const vec3& lr,
                   int sampleRate);
        Ray createViewingRay(const Sample& normSamp);
    private:
        vec3 position;
        vec3 edge1;
        vec3 edge2;
        vector< vector<vec3> > jitteredPoints;
        CRandomMersenne* randGen;
};

#endif