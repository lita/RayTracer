#include "Sampler.h"
#include <time.h>
#include <cstdlib>


/* Constructors */

Sampler::Sampler(unsigned int width, unsigned int height, unsigned int perPixel) {

	pixelWidth = width;
	pixelHeight = height;
	n = perPixel;
	i = j = 0;
	p = q = 0;

	randGen = new CRandomMersenne((uint32)time(NULL));

}


/* Destructor */

Sampler::~Sampler() {

	delete randGen;
}


/* Instance methods */

Sample Sampler::nextSample() {

	if (!hasMoreSamples())
		throw "Sampler has no more samples.";

	Sample toReturn;
	toReturn.p = p;
	toReturn.q = q;
	// Stratified (jittered) sampling (But only if n > 1).
	double eps1 = n == 1 ? 0.5 : randGen->Random();		//((double)rand()) / RAND_MAX;
	double eps2 = n == 1 ? 0.5 : randGen->Random();		//((double)rand()) / RAND_MAX;
	toReturn.horiz = i + (p + eps1) / n;
	toReturn.vert = j + (q + eps2) / n;

	// CASE: This is the last sample for the overall last pixel
	if (i == pixelWidth - 1 && j == pixelHeight - 1 && p == n -1 && q == n - 1) {
		i++;
		j++;
		p = q = 0;
	}
	// CASE: This is the last sample for the last pixel in one row
	else if (i == pixelWidth - 1 && p == n - 1 && q == n - 1) {
		i = 0;
		j++;
		p = q = 0;
	}
	// CASE: This is the last sample for a pixel in the middle of a row
	else if (p == n - 1 && q == n - 1) {
		i++;
		p = q = 0;
	}
	// CASE: This is the last sample in one row of a single pixel
	else if (q == n - 1) {
		q = 0;
		p++;
	}
	// CASE: This is a sample in the middle of a row of a single pixel
	else q++;

	return toReturn;
	
}

Sample Sampler::normalizeSample(const Sample& samp) {

	Sample toReturn;
	toReturn.horiz = samp.horiz / pixelWidth;
	toReturn.vert = samp.vert / pixelHeight;
	toReturn.p = samp.p;
	toReturn.q = samp.q;

	return toReturn;
}