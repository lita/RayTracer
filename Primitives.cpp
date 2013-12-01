#include "Primitives.h"
#include "algebra3.h"
#include <cfloat>

///////////////////////////////////////////////
//			  GeoPrimitive Class             //
///////////////////////////////////////////////

/* Constructors */

GeoPrimitive::GeoPrimitive(Shape *shape, Material* mat) {

	this->shape = shape;
	material = mat;

}

GeoPrimitive::~GeoPrimitive() {

	delete shape;

}


/* Instance methods */

bool GeoPrimitive::intersect(Ray& ray, IntersectRecord* rec) {

	// Just this for now...later on we'll need populate other
	// members of "rec" before returning.
	bool hit = shape->intersect(ray, rec);
	if (hit) {
		rec->primitive = this;
		return true;
	} else return false;

}

Reflectance GeoPrimitive::getReflectance(const vec3& point) {

	return material->getReflectance(point, shape);

}

BoundingBox GeoPrimitive::getBoundingBox() {

	return shape->getBoundingBox();

}

Primitive* GeoPrimitive::instance(const mat4& transform, Material* mat) {

	return new GeoPrimitive(new TransformedShape(shape, transform), mat);
}


///////////////////////////////////////////////
//			BoundingBoxTree Class            //
///////////////////////////////////////////////

BoundingBoxTree::BoundingBoxTree(const vector<Primitive*>& objects, int splitAxis) {

	this->splitAxis = splitAxis;
	unsigned int length = objects.size();

	if (length == 1) {
		low = objects[0];
		high = NULL;
		box = objects[0]->getBoundingBox();
	} else if (length == 2) {
		low = objects[0];
		high = objects[1];
		box = BoundingBox::combine(low->getBoundingBox(), high->getBoundingBox());
	} else {
		vector<Primitive*> lowVec;
		vector<Primitive*> highVec;
		partition(splitAxis, objects, &lowVec, &highVec);
		low = lowVec.size() > 0 ? new BoundingBoxTree(lowVec, (splitAxis + 1) % 3) : NULL;
		high = highVec.size() > 0 ? new BoundingBoxTree(highVec, (splitAxis + 1) % 3) : NULL;
		if (low == NULL)
			box = high->getBoundingBox();
		else if (high == NULL)
			box = low->getBoundingBox();
		else box = BoundingBox::combine(low->getBoundingBox(), high->getBoundingBox());
	}
}

void BoundingBoxTree::partition(int axis, const vector<Primitive*>& all, vector<Primitive*>* lowVec, vector<Primitive*>* highVec) {

	double min, max;
	min = DBL_MAX;
	max = -DBL_MAX;

	for(unsigned int i = 0; i < all.size(); i++) {
		BoundingBox bbox = all[i]->getBoundingBox();
		min = MIN(min, bbox.minCoordinate(axis));
		max = MAX(max, bbox.maxCoordinate(axis));
	}

	double pivot = (max + min) / 2;

	for (unsigned int i = 0; i < all.size(); i++) {
		BoundingBox bbox = all[i]->getBoundingBox();
		double centerCoord = (bbox.maxCoordinate(axis) + bbox.minCoordinate(axis)) / 2;
		if (centerCoord < pivot)
			lowVec->push_back(all[i]);
		else highVec->push_back(all[i]);
	}

	// Check to see if we failed to make a partition. If so, grab the closest thing
	// that isn't on the other side of the partition and throw it into the empty list.

	// CASE: Everything ended up in highVec
	if (highVec->size() == all.size()) {
		double minCoord = DBL_MAX;
		unsigned int index;
		Primitive* obj;
		for (unsigned int i = 0; i < highVec->size(); i++) {
			BoundingBox bbox = (*highVec)[i]->getBoundingBox();
			double centerCoord = (bbox.maxCoordinate(axis) + bbox.minCoordinate(axis)) / 2;
			if (centerCoord < minCoord) {
				minCoord = centerCoord;
				index = i;
				obj = (*highVec)[i];
			}
		}
		highVec->erase(highVec->begin() + index);
		lowVec->push_back(obj);
	}
	// CASE: Everything ended up in lowVec
	else if (lowVec->size() == all.size()) {
		double maxCoord = -DBL_MAX;
		unsigned int index;
		Primitive* obj;
		for (unsigned int i = 0; i < lowVec->size(); i++) {
			BoundingBox bbox = (*lowVec)[i]->getBoundingBox();
			double centerCoord = (bbox.maxCoordinate(axis) + bbox.minCoordinate(axis)) / 2;
			if (centerCoord > maxCoord) {
				maxCoord = centerCoord;
				index = i;
				obj = (*lowVec)[i];
			}
		}
		lowVec->erase(lowVec->begin() + index);
		highVec->push_back(obj);
	}
}

BoundingBoxTree::BoundingBoxTree(BoundingBoxTree* otherTree, const mat4& transMat, Material* mat) {

	splitAxis = otherTree->splitAxis;
	high = (otherTree->high == NULL ? NULL : otherTree->high->instance(transMat, mat));
	low = (otherTree->low == NULL ? NULL : otherTree->low->instance(transMat, mat));

	if (low == NULL)
		box = high->getBoundingBox();
	else if (high == NULL)
		box = low->getBoundingBox();
	else box = BoundingBox::combine(low->getBoundingBox(), high->getBoundingBox());
}

BoundingBoxTree::~BoundingBoxTree() {

	delete high;
	delete low;

}

Reflectance BoundingBoxTree::getReflectance(const vec3& point) {

	// This should never be called.
	throw "BoundingBoxTree does not implement this method.";
}

BoundingBox BoundingBoxTree::getBoundingBox() {

	return box;

}

Primitive* BoundingBoxTree::instance(const mat4& transform, Material* mat) {

	return new BoundingBoxTree(this, transform, mat);
}


bool BoundingBoxTree::intersect(Ray& ray, IntersectRecord* rec) {

	if (!box.intersect(ray, rec))
		return false;

	rec->t = ray.getUpperBound();
	bool hit1 = false, hit2 = false;
	double oldMax = rec->t;

	// CASE: Ray is moving in positive direction
	if (ray.getSign(splitAxis) == 0) {
		hit1 = (low != NULL && low->intersect(ray, rec));
		ray.setBounds(ray.getLowerBound(), rec->t);
		hit2 = (high != NULL && high->intersect(ray, rec));
		ray.setBounds(ray.getLowerBound(), oldMax);		// Reset ray bounds before returning
		return (hit1 || hit2);
	}
	// CASE: Ray is moving in negative direction
	else {
		hit1 = (high != NULL && high->intersect(ray, rec));
		ray.setBounds(ray.getLowerBound(), rec->t);
		hit2 = (low != NULL && low->intersect(ray, rec));
		ray.setBounds(ray.getLowerBound(), oldMax);		// Reset ray bounds before returning
		return (hit1 || hit2);
	}

}


///////////////////////////////////////////////
//			  MeshPrimitive Class            //
///////////////////////////////////////////////

MeshPrimitive::MeshPrimitive(Mesh* mesh, vector<Shape*> triangles, Material* mat) {

	this->mesh = mesh;
	this->mat = mat;
	lastIntersected = NULL;

	vector<Primitive*> prims;
	for (unsigned int i = 0; i < triangles.size(); i++)
		prims.push_back(new GeoPrimitive(triangles[i], mat));
	this->triangleTree = new BoundingBoxTree(prims, VZ);

}

MeshPrimitive::MeshPrimitive(MeshPrimitive* otherMesh, const mat4& transMat, Material* mat) {

	this->mesh = otherMesh->mesh;
	this->mat = mat;
	lastIntersected = NULL;
	triangleTree = (BoundingBoxTree*)otherMesh->triangleTree->instance(transMat, mat);
}

MeshPrimitive::~MeshPrimitive() {

	delete triangleTree;
}


bool MeshPrimitive::intersect(Ray& ray, IntersectRecord* rec) {

	bool hit = triangleTree->intersect(ray, rec);
	if (hit) {
		lastIntersected = rec->primitive;
		rec->primitive = this;
	}
	return hit;
}


// Assumes that we've just intersected at "point."
Reflectance MeshPrimitive::getReflectance(const vec3& point) {

	return lastIntersected->getReflectance(point);
}


BoundingBox MeshPrimitive::getBoundingBox() {

	return triangleTree->getBoundingBox();

}

Primitive* MeshPrimitive::instance(const mat4& transform, Material* mat) {

	return new MeshPrimitive(this, transform, mat);

}