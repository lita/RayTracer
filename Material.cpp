#include "Material.h"
#include <iostream>

using namespace std;

////////////////////////////////////////////////
//				  TEXTURE			          //
////////////////////////////////////////////////

Texture::Texture(string filename) {

	if (filename.find(".jpg") != string::npos)
		image = FreeImage_Load(FIF_JPEG, filename.c_str(), JPEG_ACCURATE);
	else if (filename.find(".png") != string::npos)
		image = FreeImage_Load(FIF_PNG, filename.c_str(), PNG_IGNOREGAMMA);
	else {
		cout << endl;
		cerr << "Error: Unsupported texture format in " << filename << endl;
		exit(1);
	}

	width = FreeImage_GetWidth(image);
	height = FreeImage_GetHeight(image);

}


rgb Texture::getColor(const vec2& uvCoord) {

	unsigned int i = ((unsigned int)(uvCoord[0] * width)) % width;
	unsigned int j = ((unsigned int)(uvCoord[1] * height)) % height;

	RGBQUAD color;
	FreeImage_GetPixelColor(image, i, j, &color);
	return rgb(color.rgbRed / 255.0, color.rgbGreen / 255.0, color.rgbBlue / 255.0);
}


////////////////////////////////////////////////
//				  MATERIAL				      //
////////////////////////////////////////////////

/* Constructors */

Material::Material() {

	myReflectance.kA = myReflectance.kD = myReflectance.kR =
		myReflectance.kS = rgb();
	myReflectance.pExp = 0;
}

Material::Material(Reflectance reflec) {

	myReflectance = reflec;

}


/* Instance methods */

Reflectance Material::getReflectance(const vec3& point, Shape* shape) {

	return myReflectance;

}


////////////////////////////////////////////////
//			   TEXTUREDMATERIAL				  //
////////////////////////////////////////////////

TexturedMaterial::TexturedMaterial(Reflectance reflec, Texture* texMap, bool rough)
: Material(reflec) {

	texture = texMap;
	this->rough = rough;

}


Reflectance TexturedMaterial::getReflectance(const vec3& point, Shape* shape) {

	rgb texColor = texture->getColor(shape->getTextureCoordinate(point));
	return multiply(Material::getReflectance(point, shape), texColor, rough);

}

Reflectance TexturedMaterial::multiply(const Reflectance& ref, const rgb& color, bool rough) {

	Reflectance toReturn = ref;
	toReturn.kA  = toReturn.kA * color;
	toReturn.kD  = toReturn.kD * color;
	if (rough) {
		toReturn.kS  = toReturn.kS * color;
		toReturn.kR  = toReturn.kR * color;
	}

	return toReturn;
}