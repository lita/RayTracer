#ifndef PRIMITIVESH
#define PRIMITIVESH

// Forward declarations
class Shape;
typedef struct intersect_record_struct IntersectRecord;

#include "Ray.h"
#include "IntersectRecord.h"
#include "Shapes.h"
#include "Material.h"
#include "algebra3.h"
#include <vector>

using namespace std;


/* Abstract class for all objects in a Scene. */
class Primitive {

public:

	/* Default dtor */
	virtual ~Primitive() {}
	/* Virtual intersect method */
	virtual bool intersect(Ray& ray, IntersectRecord* rec) = 0;
	/* Virtual getter methods */
	virtual Reflectance getReflectance(const vec3& point) = 0;
	virtual BoundingBox getBoundingBox() = 0;
	/* Virtual copy method */
	virtual Primitive* instance(const mat4& transform, Material* mat) = 0;

};


/* Class for geometric primitives in a Scene. */
class GeoPrimitive : public Primitive {

public:

	/* Constructor */
	GeoPrimitive(Shape* shape, Material* mat);

	~GeoPrimitive();

	/* Instance methods */
	bool intersect(Ray& ray, IntersectRecord* rec);
	Reflectance getReflectance(const vec3& point);
	BoundingBox getBoundingBox();
	Primitive* instance(const mat4& transform, Material* mat);

private:

	/* Instance vars */
	Shape* shape;
	Material* material;

};


/* Class for bounding volume heirarchies. */
class BoundingBoxTree : public Primitive {

public:

	/* Constructor */
	BoundingBoxTree(const vector<Primitive*>& objects, int splitAxis);

	~BoundingBoxTree();

	/* Instance methods */
	bool intersect(Ray& ray, IntersectRecord* rec);
	Reflectance getReflectance(const vec3& point);
	BoundingBox getBoundingBox();
	Primitive* instance(const mat4& transform, Material* mat);

	/* Static methods */
	static void partition(int axis, const vector<Primitive*>& all, vector<Primitive*>* lowVec, vector<Primitive*>* highVec);

private:

	BoundingBoxTree(BoundingBoxTree* otherTree, const mat4& transMat, Material* mat);
	/* Instance vars */
	BoundingBox box;
	int splitAxis;
	Primitive* high;
	Primitive* low;

};


/* Class for mesh objects */
class MeshPrimitive : public Primitive {

public:

	MeshPrimitive(Mesh* mesh, vector<Shape*> triangles, Material* mat);
	~MeshPrimitive();
	bool intersect(Ray& ray, IntersectRecord* rec);
	Reflectance getReflectance(const vec3& point);
	BoundingBox getBoundingBox();
	Primitive* instance(const mat4& transform, Material* mat);

private:

	MeshPrimitive(MeshPrimitive* otherMesh, const mat4& transMat, Material* mat);
	Mesh* mesh;
	BoundingBoxTree* triangleTree;
	Material* mat;
	Primitive* lastIntersected;
};



#endif