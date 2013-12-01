/*
 *  Light.h
 *  Classes relating to all the different lights. 
 *
 *  Created by Lita Cho on 9/22/08.
 * 
 */

#ifndef LIGHTSH
#define LIGHTSH

#include "Ray.h"
#include "rgb.h"
#include "algebra3.h"
#include "randomc.h"
#include <vector>

using namespace std;

/*****************************************************************************/
/*                      Abstract Light Class								 */
/******************* **********************************************************/
/* General abstract light class used to dervie all other lights for the ray
 * tracer.
 */
class Light {
    public:
        // Default Constructor
        Light();
        
        // Default Destructor
        virtual ~Light();
        
        // Constructs a light given the position and the color.
        // Parameters: 
        //      x, y, z - Position of the light.
        //      color - The RGB values regarding the color of the light
        Light(const rgb color);
        
        // Returns the color of the light.
        const rgb getColor();

		// Gets the shadow ray from STARTPOINT to this light object given a
		// certain shadow BIAS.
		virtual Ray getShadowRay(const vec3& startPoint, double bias, Ray& viewRay) = 0;
    
    protected:
        rgb illumination;

		// Returns the vector of the light given a point on the object.
        // Dervie class must override.
        // Parameters:
        //      point - the point on the surface where you want to evaluate
        //              if the light is hitting that area.
};

/*****************************************************************************/
/*                          Point Light Class                                */
/*****************************************************************************/
class PointLight: public Light {
    // Constructs a point light given the position and the color.
    // Parameters: 
    //      x, y, z - Position of the light.
    //      color - The RGB values regarding the color of the light
    public:
		PointLight(double x, double y, double z, const rgb color);
		Ray getShadowRay(const vec3& startPoint, double bias, Ray& viewRay);

	private:
		vec3 position;
		vec3 getIncidence(const vec3 &point);
};

/*****************************************************************************/
/*                     Directional Light Class                               */
/*****************************************************************************/
class DirectionalLight: public Light {
    // Constructs a point light given the position and the color.
    // Parameters: 
    //      x, y, z - Position of the light.
    //      color - The RGB values regarding the color of the light
    public:
		DirectionalLight(double x, double y, double z, const rgb color);
		Ray getShadowRay(const vec3& startPoint, double bias, Ray& viewRay);

	private:
		vec3 direction;
		vec3 getIncidence(const vec3 &point);
};

/*****************************************************************************/
/*						  Area Light Class                                   */
/*****************************************************************************/

class AreaLight: public Light {

public:
	
	AreaLight(const vec3& position, const vec3& edge1, const vec3& edge2, const rgb color, int sampleRate);
	~AreaLight();
	Ray getShadowRay(const vec3& startPoint, double bias, Ray& viewRay);

private:

	vec3 position;
	vec3 edge1;
	vec3 edge2;
	CRandomMersenne* randGen;
	vector< vector<vec3> > jitteredPoints;
	vec3 getIncidence(const vec3 &point, Ray& viewRay);

};


#endif