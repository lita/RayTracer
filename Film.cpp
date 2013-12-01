#include "Film.h"
#include "FreeImage.h"
#include <iostream>

using namespace std;


/* Constructors */

Film::Film(unsigned int imageWidth, unsigned int imageHeight) {

	sampleGrid = new vector<rgb>*[imageWidth];
	for(unsigned int i = 0; i < imageWidth; i++)
		sampleGrid[i] = new vector<rgb>[imageHeight];

	pixelWidth = imageWidth;
	pixelHeight = imageHeight;

}


/* Destructor */

Film::~Film() {

	for (unsigned int i = 0; i < pixelWidth; i++)
		delete [] sampleGrid[i];
	delete [] sampleGrid;

}


/* Instance methods */

void Film::commit(const Sample& samp, const rgb& color) {

	unsigned int i = (unsigned int)samp.horiz;
	unsigned int j = (unsigned int)samp.vert;
	sampleGrid[i][j].push_back(color);

}

void Film::writeImage(string filename) {

	filename += ".png";
	// [START] WRITE IMAGE
	cout << "Writing to file \"" << filename << "\"...";

	FIBITMAP* image = FreeImage_Allocate(pixelWidth, pixelHeight, 24);
	RGBQUAD pixel;
	
	for (unsigned int i = 0; i < pixelWidth; i++)
		for (unsigned int j = 0; j < pixelHeight; j++) {
			vector<rgb> colorSamples = sampleGrid[i][j];
			rgb pixelColor(0,0,0);
			for (unsigned int k = 0; k < colorSamples.size(); k++)
				pixelColor += colorSamples[k];
			pixelColor /= colorSamples.size();
			pixelColor.normalize();
			pixel.rgbRed = (BYTE)(pixelColor[0] * 255);
			pixel.rgbGreen = (BYTE)(pixelColor[1] * 255);
			pixel.rgbBlue = (BYTE)(pixelColor[2] * 255);
			FreeImage_SetPixelColor(image, i, j, &pixel);
		}
	FreeImage_Save(FIF_PNG, image, filename.c_str(), PNG_IGNOREGAMMA);

	// [END] WRITE IMAGE
	cout << "DONE" << endl;

}