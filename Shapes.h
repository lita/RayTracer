/*
 *  Shape.h
 *  RayTracer
 *  
 *  Shape classes
 *
 *  Created by Lita Cho on 9/23/08.
 *
 */

#ifndef SHAPESH
#define SHAPESH

//Forward declarations
class Ray;

#include "algebra3.h"
#include "Ray.h"
#include <vector>

using namespace std;

// Forward declarations
class BoundingBox;
typedef struct intersect_record_struct IntersectRecord;

class Shape {

    public:
        // Default Constructor
        Shape();
        // Default Destructor
        virtual ~Shape();
        // If object intersects with a ray, return true.
        virtual bool intersect(Ray& ray, IntersectRecord* rec) = 0;
		// Get the bounding box for this shape
		virtual BoundingBox getBoundingBox() = 0;
		// Get the texture coordinate for this shape given a point on the shape.
		virtual vec2 getTextureCoordinate(const vec3& point) = 0;

};


class BoundingBox : public Shape {

public:
	BoundingBox();
	BoundingBox(const vec3& min, const vec3& max);
	bool intersect(Ray& ray, IntersectRecord* rec);
	BoundingBox getBoundingBox();
	vec2 getTextureCoordinate(const vec3& point);
	void transform(const mat4& transformMatrix);
	double minCoordinate(int axis);
	double maxCoordinate(int axis);

	static BoundingBox combine(const BoundingBox& box1, const BoundingBox& box2);

private:
    // The min point is going to be stored in bounds[0] and the max is stored
    // at bounds[1].
    vec3 bounds[2];
};


class Sphere : public Shape {

    public:
        Sphere(double radius, const vec3& center);
        bool intersect(Ray& ray, IntersectRecord* rec);
		BoundingBox getBoundingBox();
		vec2 getTextureCoordinate(const vec3& point);

		static Sphere unitSphere;

    private:
        double getDiscriminant(const vec3& origin, const vec3& direction);
		void getNormal(Ray& ray, IntersectRecord* rec);
    
    private:
		vec3 center;
        double radius;
        
};


class TransformedShape : public Shape {

public:
	TransformedShape(Shape* shape, const mat4& transform);
	bool intersect(Ray& ray, IntersectRecord* rec);
	BoundingBox getBoundingBox();
	vec2 getTextureCoordinate(const vec3& point);

protected:
	vec3 transformNormal(const vec3& normal);

	Shape* shape;
	mat4 inverseTransform;

};


class Ellipsoid : public TransformedShape {

public:
	Ellipsoid(const mat4& transform);

};


class Triangle : public Shape {

    public:
        Triangle (const vec3& a, const vec3& b, const vec3& c);
        bool intersect(Ray& ray, IntersectRecord* rec);
		BoundingBox getBoundingBox();
		vec2 getTextureCoordinate(const vec3& point);

    private:
		void getNormal(Ray& ray, IntersectRecord* rec);
		vec3 a;
        vec3 b;
        vec3 c;
		mat3 barycentricFinder;
        
};


struct Mesh {

	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec2> textures;

};


class MeshTriangle : public Shape {

public:

	MeshTriangle(Mesh* mesh, int vertI[], int normI[], int texI[]);
	virtual bool intersect(Ray& ray, IntersectRecord* rec);
	BoundingBox getBoundingBox();
	vec2 getTextureCoordinate(const vec3& point);

protected:

	void getNormal(Ray& ray, IntersectRecord* rec);
	Mesh* mesh;
	int vertI[3];
	int normI[3];
	int texI[3];
	mat3 barycentricFinder;

};


class WireframeTriangle : public MeshTriangle {

public:

	WireframeTriangle(Mesh* mesh, int vertI[], int normI[], int texI[]);
	bool intersect(Ray& ray, IntersectRecord* rec);
};


#endif