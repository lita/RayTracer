#ifndef RGBH
#define RGBH

#include "algebra3.h"

// Red,Green,Blue color object.
class rgb : public vec3 { 
public:
	// Constructors
	rgb();															// Default constructor
	rgb(const double r, const double g, const double b);			// Basic constructor
	rgb(const rgb& c);												// Copy constructor

	// Operators
	rgb& operator	= ( const rgb& c );								// Assignment of an rgb

	// Friends
	friend rgb operator * (const rgb& a, const rgb& b);				// Element-wise multiplication
	friend rgb operator - (const rgb& c);							// -v1
	friend rgb operator + (const rgb& a, const rgb& b);				// v1 + v2
	friend rgb operator - (const rgb& a, const rgb& b);				// v1 - v2
	friend rgb operator * (const rgb& a, const double d);			// v1 * 3.0
	friend rgb operator * (const double d, const rgb& a);			// 3.0 * v1
	friend rgb operator / (const rgb& a, const double d);			// v1 / 3.0
	friend int operator == (const rgb& a, const rgb& b);			// v1 == v2 ?
	friend int operator != (const rgb& a, const rgb& b);			// v1 != v2 ?

	// Special function
	rgb& normalize();												// All components <= 1.0

	static const rgb black;
};

/* Constructors */

inline rgb::rgb() {
	n[VX] = n[VY] = n[VZ] = 0.0;
}

inline rgb::rgb(const double x, const double y, const double z)
: vec3(x, y, z) {}

inline rgb::rgb(const rgb& v)
: vec3(v) {}


/* Operators */

inline rgb& rgb::operator = (const rgb& v)
{ n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ]; return *this; }


/* Friends */

inline rgb operator * (const rgb& a, const rgb& b) {
	return rgb(a.n[VX]*b.n[VX], a.n[VY]*b.n[VY], a.n[VZ]*b.n[VZ]);
}

inline rgb operator - (const rgb& a)
{  return rgb(-a.n[VX],-a.n[VY],-a.n[VZ]); }

inline rgb operator + (const rgb& a, const rgb& b)
{ return rgb(a.n[VX]+ b.n[VX], a.n[VY] + b.n[VY], a.n[VZ] + b.n[VZ]); }

inline rgb operator - (const rgb& a, const rgb& b)
{ return rgb(a.n[VX]-b.n[VX], a.n[VY]-b.n[VY], a.n[VZ]-b.n[VZ]); }

inline rgb operator * (const rgb& a, const double d)
{ return rgb(d*a.n[VX], d*a.n[VY], d*a.n[VZ]); }

inline rgb operator * (const double d, const rgb& a)
{ return a*d; }

inline rgb operator / (const rgb& a, const double d)
{ double d_inv = 1.0/d; return rgb(a.n[VX]*d_inv, a.n[VY]*d_inv,
  a.n[VZ]*d_inv); }

inline int operator == (const rgb& a, const rgb& b)
{ return (a.n[VX] == b.n[VX]) && (a.n[VY] == b.n[VY]) && (a.n[VZ] == b.n[VZ]);
}

inline int operator != (const rgb& a, const rgb& b)
{ return !(a == b); }


#endif