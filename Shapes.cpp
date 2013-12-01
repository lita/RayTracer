/*
 *  Shape.cpp
 *  RayTracer
 *
 *  Created by Lita Cho on 9/23/08.
 *
 */

#include "Shapes.h"
#include "IntersectRecord.h"
#include <cfloat>
#include <cmath>

#define WIREFRAME_THRESHOLD 0.01


Shape::Shape() {}

Shape::~Shape() {}

/****************************/
/*      BoundingBox         */
/****************************/

BoundingBox::BoundingBox() {

	bounds[0] = vec3(0,0,0);
	bounds[1] = vec3(0,0,0);

}

BoundingBox::BoundingBox(const vec3 &min, const vec3 &max) {

    bounds[0] = min;
	bounds[1] = max;

}

bool BoundingBox::intersect(Ray& ray, IntersectRecord* rec) {
    double tmin, tmax, tymin, tymax, tzmin, tzmax;
    vec3 direction = ray.getDirection();
    vec3 origin = ray.getOrigin();
    vec3 inverse = ray.getInverseDirection();
    tmin = ((bounds[ray.getSign(0)][0] - origin[0]) * inverse[0]);
    tmax = ((bounds[1-ray.getSign(0)][0] - origin[0]) * inverse[0]);

    tymin = ((bounds[ray.getSign(1)][1] - origin[1]) * inverse[1]);
    tymax = ((bounds[1-ray.getSign(1)][1] - origin[1]) * inverse[1]);
       
    if ( (tmin > tymax) || (tymin > tmax) ) 
        return false; 
    if (tymin > tmin) 
        tmin = tymin; 
    if (tymax < tmax) 
        tmax = tymax; 
        
    tzmin = ((bounds[ray.getSign(2)][2] - origin[2]) * inverse[2]);
    tzmax = ((bounds[1-ray.getSign(2)][2] - origin[2]) * inverse[2]);

    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 
    if (tzmin > tmin) 
        tmin = tzmin; 
    if (tzmax < tmax)
        tmax = tzmax; 
	return ( tmin < ray.getUpperBound() && tmax > ray.getLowerBound());
}

void BoundingBox::transform(const mat4& transformMatrix) {

	/* The definitely correct (but slow) way: */
	vec3 points[8];
	points[0] = vec3(transformMatrix * vec4(bounds[0][0],bounds[0][1],bounds[0][2],1));
	points[1] = vec3(transformMatrix * vec4(bounds[0][0],bounds[0][1],bounds[1][2],1));
	points[2] = vec3(transformMatrix * vec4(bounds[0][0],bounds[1][1],bounds[0][2],1));
	points[3] = vec3(transformMatrix * vec4(bounds[0][0],bounds[1][1],bounds[1][2],1));
	points[4] = vec3(transformMatrix * vec4(bounds[1][0],bounds[0][1],bounds[0][2],1));
	points[5] = vec3(transformMatrix * vec4(bounds[1][0],bounds[0][1],bounds[1][2],1));
	points[6] = vec3(transformMatrix * vec4(bounds[1][0],bounds[1][1],bounds[0][2],1));
	points[7] = vec3(transformMatrix * vec4(bounds[1][0],bounds[1][1],bounds[1][2],1));

	bounds[0][0] = bounds[0][1] = bounds[0][2] = DBL_MAX;
	bounds[1][0] = bounds[1][1] = bounds[1][2] = -DBL_MAX;

	for (int i = 0; i < 8; i++) {
		bounds[0][0] = MIN(bounds[0][0], points[i][0]);
		bounds[0][1] = MIN(bounds[0][1], points[i][1]);
		bounds[0][2] = MIN(bounds[0][2], points[i][2]);
		bounds[1][0] = MAX(bounds[1][0], points[i][0]);
		bounds[1][1] = MAX(bounds[1][1], points[i][1]);
		bounds[1][2] = MAX(bounds[1][2], points[i][2]);
	}

}

BoundingBox BoundingBox::getBoundingBox() {

	return *this;

}

vec2 BoundingBox::getTextureCoordinate(const vec3& point) {

	// This should never be called.
	throw "BoundingBox does not implement this method.";
}

double BoundingBox::minCoordinate(int axis) {

	return bounds[0][axis];

}

double BoundingBox::maxCoordinate(int axis) {

	return bounds[1][axis];

}

BoundingBox BoundingBox::combine(const BoundingBox& box1, const BoundingBox& box2) {

	BoundingBox comboBox;

	comboBox.bounds[0][0] = MIN(box1.bounds[0][0], box2.bounds[0][0]);
	comboBox.bounds[0][1] = MIN(box1.bounds[0][1], box2.bounds[0][1]);
	comboBox.bounds[0][2] = MIN(box1.bounds[0][2], box2.bounds[0][2]);

	comboBox.bounds[1][0] = MAX(box1.bounds[1][0], box2.bounds[1][0]);
	comboBox.bounds[1][1] = MAX(box1.bounds[1][1], box2.bounds[1][1]);
	comboBox.bounds[1][2] = MAX(box1.bounds[1][2], box2.bounds[1][2]);

	return comboBox;
}


/****************************/
/*         Sphere           */
/****************************/
Sphere::Sphere(double radius, const vec3& center) {

	this->radius = radius;
	this->center = center;
}
    
bool Sphere::intersect(Ray& ray, IntersectRecord* rec) {

	vec3 origin = ray.getOrigin();
    vec3 direction = ray.getDirection();
    
    // If discriminant is less than zero, then the ray didn't
    // hit anything.
	double discriminant = getDiscriminant(origin, direction);
    if (discriminant < 0) 
        return false;
    
	// Quadratic formula!
	double leftTerm = -direction * (origin - center);
	double rightTerm = sqrt(discriminant);
	double denominator = direction * direction;
	double tNeg = (leftTerm - rightTerm) / denominator;
	double tPos = (leftTerm + rightTerm) / denominator;

	// If either of the roots is within the ray's intersection
	// bounds, use smaller one. Otherwise, the ray didn't hit.
	if (ray.isWithinBounds(tNeg))
		rec->t = tNeg;
	else if (ray.isWithinBounds(tPos))
		rec->t = tPos;
	else return false;

	// Fill in the rest of the hit record.
	rec->point = ray.intersectionPoint(rec->t);
    getNormal(ray, rec);
	return true;
}

BoundingBox Sphere::getBoundingBox() {

	vec3 radiusVec(radius,radius,radius);
	return BoundingBox(center - radiusVec, center + radiusVec);
}

vec2 Sphere::getTextureCoordinate(const vec3& point) {

	double theta = acos((point[VY] - center[VY]) / radius);
	double phi = atan2(point[VZ] - center[VY], point[VX] - center[VX]);
	phi = (phi < 0 ? phi + 2*M_PI : phi);
	double u = 1 - (phi / (2 * M_PI));
	double v = (M_PI - theta) / M_PI;
	return vec2(u,v);

}

void Sphere::getNormal(Ray& ray, IntersectRecord *rec) {
    // Find Normal
    vec3 surfaceNormal = rec->point - center;
    surfaceNormal.normalize();

    //Get the norm of the vector between the ray and the center of sphere.
    vec3 originToSphere = ray.getOrigin() - center;
    
    // If the length of the vector is less than the radius, ray originated
    // from inside the sphere. Thus we need to negate the normal.
	if (originToSphere.length() < radius)
        surfaceNormal = -surfaceNormal;

    rec->surfaceNormal = surfaceNormal;
}

// Gets the discriminat givin the origin and direction of the ray
double Sphere::getDiscriminant(const vec3& origin, const vec3& direction) {
    double scalarA = (direction * (origin - center)) * (direction * (origin - center));
    double scalarB = (origin - center) * (origin - center) - radius * radius;
    return scalarA - (direction * direction) * scalarB;
}

Sphere Sphere::unitSphere = Sphere(1.0, vec3(0,0,0));


/****************************/
/*    Transformed Shape     */
/****************************/

TransformedShape::TransformedShape(Shape* shape, const mat4& transform) {

	this->shape = shape;
	inverseTransform = transform.inverse();
}

bool TransformedShape::intersect(Ray& ray, IntersectRecord* rec) {
    Ray temp = inverseTransform * ray;
    bool result = shape->intersect(temp, rec);
	if (result) {
		rec->point = ray.intersectionPoint(rec->t);
		rec->surfaceNormal = transformNormal(rec->surfaceNormal);
	}

	return result;
}

BoundingBox TransformedShape::getBoundingBox() {

	BoundingBox bbox = shape->getBoundingBox();
	bbox.transform(inverseTransform.inverse());
	return bbox;

}

vec2 TransformedShape::getTextureCoordinate(const vec3& point) {

	vec3 transPoint = vec3(inverseTransform * vec4(point));
	return shape->getTextureCoordinate(transPoint);
}

vec3 TransformedShape::transformNormal(const vec3& normal) {

	vec3 transNorm = vec3(inverseTransform.transpose() * vec4(normal, 0.0), VW);
	transNorm.normalize();
	return transNorm;

}


/****************************/
/*        Ellipsoid         */
/****************************/

Ellipsoid::Ellipsoid(const mat4& transform)
: TransformedShape(&Sphere::unitSphere, transform) {}


/****************************/
/*         Triangle         */
/****************************/
Triangle::Triangle (const vec3& a, const vec3& b, const vec3& c) {
    this->a = a;
    this->b = b;
    this->c = c;

	barycentricFinder = mat3(a, b, c);
	barycentricFinder = barycentricFinder.transpose().inverse();
}

bool Triangle::intersect(Ray& ray, IntersectRecord* rec) {

    vec3 origin = ray.getOrigin();
    vec3 direction = ray.getDirection();

    vec3 row1 = vec3(a[0] - b[0], a[0] - c[0], direction[0]);
    vec3 row2 = vec3(a[1] - b[1], a[1] - c[1], direction[1]);
    vec3 row3 = vec3(a[2] - b[2], a[2] - c[2], direction[2]);
    mat3 linearEq = mat3(row1, row2, row3);

    if (fabs(linearEq.determinant()) < 0.000001) 
        return false;
    
    mat3 inverse = linearEq.inverse();
    mat3 idenity = inverse * linearEq;
    
    vec3 answer = a - origin;
    vec3 solution = inverse * answer;
    double t = solution[2];
    double gamma = solution[1];
    double beta = solution[0];
    
    if (!ray.isWithinBounds(t))
        return false;
    
    if (gamma < 0 || beta < 0 || gamma + beta > 1)
        return false;
    
    rec->t = t;
	rec->point = ray.intersectionPoint(rec->t);
    getNormal(ray, rec);
    return true;
    
}

BoundingBox Triangle::getBoundingBox() {

	vec3 min(DBL_MAX, DBL_MAX, DBL_MAX);
	vec3 max(-DBL_MAX, -DBL_MAX, -DBL_MAX);
	// Vertex a
	min[0] = a[0] < min[0] ? a[0] : min[0];
	min[1] = a[1] < min[1] ? a[1] : min[1];
	min[2] = a[2] < min[2] ? a[2] : min[2];
	max[0] = a[0] > max[0] ? a[0] : max[0];
	max[1] = a[1] > max[1] ? a[1] : max[1];
	max[2] = a[2] > max[2] ? a[2] : max[2];
	// Vertex b
	min[0] = b[0] < min[0] ? b[0] : min[0];
	min[1] = b[1] < min[1] ? b[1] : min[1];
	min[2] = b[2] < min[2] ? b[2] : min[2];
	max[0] = b[0] > max[0] ? b[0] : max[0];
	max[1] = b[1] > max[1] ? b[1] : max[1];
	max[2] = b[2] > max[2] ? b[2] : max[2];
	// Vertex c
	min[0] = c[0] < min[0] ? c[0] : min[0];
	min[1] = c[1] < min[1] ? c[1] : min[1];
	min[2] = c[2] < min[2] ? c[2] : min[2];
	max[0] = c[0] > max[0] ? c[0] : max[0];
	max[1] = c[1] > max[1] ? c[1] : max[1];
	max[2] = c[2] > max[2] ? c[2] : max[2];

	return BoundingBox(min, max);

}

vec2 Triangle::getTextureCoordinate(const vec3& point) {

	vec3 abg = barycentricFinder * point;
	return vec2(abg[1], abg[2]);

}

void Triangle::getNormal(Ray& ray, IntersectRecord* rec) {
    vec3 normal = (b - a) ^ (b - c);
    normal.normalize();
    
    vec3 direction = -ray.getDirection();
    if (normal * direction < 0) {
         normal = -normal;
    }
    rec->surfaceNormal = normal;
    
}


/****************************/
/*	     MeshTriangle       */
/****************************/

MeshTriangle::MeshTriangle(Mesh* mesh, int vertI[], int normI[], int texI[]) {

	this->mesh = mesh;
	this->vertI[0] = vertI[0];
	this->vertI[1] = vertI[1];
	this->vertI[2] = vertI[2];
	this->normI[0] = normI[0];
	this->normI[1] = normI[1];
	this->normI[2] = normI[2];
	this->texI[0] = texI[0];
	this->texI[1] = texI[1];
	this->texI[2] = texI[2];

	barycentricFinder = mat3(mesh->vertices[vertI[0]], mesh->vertices[vertI[1]], mesh->vertices[vertI[2]]);
	barycentricFinder = barycentricFinder.transpose().inverse();
}


bool MeshTriangle::intersect(Ray& ray, IntersectRecord* rec) {

	// Get vertices
	vec3 a = mesh->vertices[vertI[0]];
	vec3 b = mesh->vertices[vertI[1]];
	vec3 c = mesh->vertices[vertI[2]];

	vec3 origin = ray.getOrigin();
    vec3 direction = ray.getDirection();

    vec3 row1 = vec3(a[0] - b[0], a[0] - c[0], direction[0]);
    vec3 row2 = vec3(a[1] - b[1], a[1] - c[1], direction[1]);
    vec3 row3 = vec3(a[2] - b[2], a[2] - c[2], direction[2]);
    mat3 linearEq = mat3(row1, row2, row3);

    if (fabs(linearEq.determinant()) < 0.000001) 
        return false;
    
    mat3 inverse = linearEq.inverse();
    mat3 idenity = inverse * linearEq;
    
    vec3 answer = a - origin;
    vec3 solution = inverse * answer;
    double t = solution[2];
    double gamma = solution[1];
    double beta = solution[0];
    
    if (!ray.isWithinBounds(t))
        return false;
    
    if (gamma < 0 || beta < 0 || gamma + beta > 1)
        return false;
    
    rec->t = t;
	rec->point = ray.intersectionPoint(rec->t);
    getNormal(ray, rec);
    return true;

}

void MeshTriangle::getNormal(Ray& ray, IntersectRecord* rec) {

	vec3 abg = barycentricFinder * rec->point;
	rec->surfaceNormal = 
		abg[0]*mesh->normals[normI[0]] +
		abg[1]*mesh->normals[normI[1]] +
		abg[2]*mesh->normals[normI[2]];
	rec->surfaceNormal.normalize();
}


BoundingBox MeshTriangle::getBoundingBox() {

	// Get vertices
	vec3 a = mesh->vertices[vertI[0]];
	vec3 b = mesh->vertices[vertI[1]];
	vec3 c = mesh->vertices[vertI[2]];

	vec3 min(DBL_MAX, DBL_MAX, DBL_MAX);
	vec3 max(-DBL_MAX, -DBL_MAX, -DBL_MAX);
	// Vertex a
	min[0] = a[0] < min[0] ? a[0] : min[0];
	min[1] = a[1] < min[1] ? a[1] : min[1];
	min[2] = a[2] < min[2] ? a[2] : min[2];
	max[0] = a[0] > max[0] ? a[0] : max[0];
	max[1] = a[1] > max[1] ? a[1] : max[1];
	max[2] = a[2] > max[2] ? a[2] : max[2];
	// Vertex b
	min[0] = b[0] < min[0] ? b[0] : min[0];
	min[1] = b[1] < min[1] ? b[1] : min[1];
	min[2] = b[2] < min[2] ? b[2] : min[2];
	max[0] = b[0] > max[0] ? b[0] : max[0];
	max[1] = b[1] > max[1] ? b[1] : max[1];
	max[2] = b[2] > max[2] ? b[2] : max[2];
	// Vertex c
	min[0] = c[0] < min[0] ? c[0] : min[0];
	min[1] = c[1] < min[1] ? c[1] : min[1];
	min[2] = c[2] < min[2] ? c[2] : min[2];
	max[0] = c[0] > max[0] ? c[0] : max[0];
	max[1] = c[1] > max[1] ? c[1] : max[1];
	max[2] = c[2] > max[2] ? c[2] : max[2];

	return BoundingBox(min, max);

}

vec2 MeshTriangle::getTextureCoordinate(const vec3& point) {

	vec3 abg = barycentricFinder * point;
	return 
		abg[0]*mesh->textures[texI[0]] +
		abg[1]*mesh->textures[texI[1]] +
		abg[2]*mesh->textures[texI[2]];

}


/****************************/
/*	   WireframeTriangle    */
/****************************/

WireframeTriangle::WireframeTriangle(Mesh* mesh, int vertI[], int normI[], int texI[])
: MeshTriangle(mesh, vertI, normI, texI) {}

bool WireframeTriangle::intersect(Ray& ray, IntersectRecord* rec) {

		// Get vertices
	vec3 a = mesh->vertices[vertI[0]];
	vec3 b = mesh->vertices[vertI[1]];
	vec3 c = mesh->vertices[vertI[2]];

	vec3 origin = ray.getOrigin();
    vec3 direction = ray.getDirection();

    vec3 row1 = vec3(a[0] - b[0], a[0] - c[0], direction[0]);
    vec3 row2 = vec3(a[1] - b[1], a[1] - c[1], direction[1]);
    vec3 row3 = vec3(a[2] - b[2], a[2] - c[2], direction[2]);
    mat3 linearEq = mat3(row1, row2, row3);

    if (fabs(linearEq.determinant()) < 0.000001) 
        return false;
    
    mat3 inverse = linearEq.inverse();
    mat3 idenity = inverse * linearEq;
    
    vec3 answer = a - origin;
    vec3 solution = inverse * answer;
    double t = solution[2];
    double gamma = solution[1];
    double beta = solution[0];
    
    if (!ray.isWithinBounds(t))
        return false;
    
    if (gamma < 0 || beta < 0 || gamma + beta > 1)
        return false;

	// Wireframe constraints
	if (!(gamma < WIREFRAME_THRESHOLD || beta < WIREFRAME_THRESHOLD ||
		gamma + beta > 1 - WIREFRAME_THRESHOLD))
		return false;
    
    rec->t = t;
	rec->point = ray.intersectionPoint(rec->t);
    getNormal(ray, rec);
    return true;

}