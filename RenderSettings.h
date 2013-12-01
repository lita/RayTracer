#ifndef RENDERSETTINGSH
#define RENDERSETTINGSH

#include <string>

using namespace std;

/* RenderSettings structs hold all the information necessary
   to render a Scene. */
typedef struct render_settings_struct {

	string filename;
	unsigned int pixelWidth;
	unsigned int pixelHeight;
	unsigned int sqrtSamplesPerPixel;
	unsigned int recursionDepth;
	double rayBias;
    unsigned int refractionDepth;

} RenderSettings;


#endif