#ifndef INTERSECTRECORDH
#define INTERSECTRECORDH

// Forward declarations
class Primitive;

#include "algebra3.h"
#include "Primitives.h"


/* IntersectRecords hold all the information one
   might want to know about the intersection between
   a ray and a primitive. */
typedef struct intersect_record_struct {

	double t;					// t-value at which ray intersected
	vec3 point;					// Point at which ray intersected
	vec3 surfaceNormal;			// Surface normal at intersection point
	Primitive* primitive;		// Primitive that was intersected
	// ...etc.

} IntersectRecord;

#endif