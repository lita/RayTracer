#ifndef MATERIAL
#define MATERIAL

#include "algebra3.h"
#include "rgb.h"
#include "Shapes.h"
#include "FreeImage.h"
#include <string>

using namespace std;


/* Simple struct that holds reflectance coefficients
   for a given material. */
typedef struct reflectance_struct {

	rgb kA;				// Ambient
	rgb kD;				// Diffuse
	rgb kS;				// Specular reflection
	rgb kR;				// Mirror reflection
    rgb kT;             // Amount of transperancy
    double indexOfRefraction; // index of refraction
	double pExp;		// Phong exponent

} Reflectance;


/* Texture objects provide access to a texture map. */
class Texture {

public:

	Texture(string filename);
	rgb getColor(const vec2& uvCoord);

private:

	FIBITMAP* image;
	unsigned int height;
	unsigned int width;

};


/* Material objects hold information one might need
   about the material of an object in a Scene. */
class Material {

protected:

	/* Instance vars */
	Reflectance myReflectance;

public:

	/* Constructors */
	Material();
	Material(Reflectance reflec);

	/* Instance methods */
	virtual Reflectance getReflectance(const vec3& point, Shape* shape);
};


class TexturedMaterial : public Material {

public:
	
	TexturedMaterial(Reflectance reflec, Texture* texMap, bool rough);
	Reflectance getReflectance(const vec3& point, Shape* shape);

private:

	Texture* texture;
	bool rough;
	static Reflectance multiply(const Reflectance& ref, const rgb& color, bool rough);

};


#endif