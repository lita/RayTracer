#ifndef OBJPARSERH
#define OBJPARSERH
/*
 *  objParser.h
 *  RayTracer
 *  
 *  Basic OBJ parser that just parses the triangles and throws away normals of triangles.
 *
 */
 
#include "Primitives.h"
#include "Material.h"
#include "algebra3.h"
#include <string>
#include <vector>

using namespace std;
 
class ObjParser {

    public:
        ObjParser(string filename, Material* mat, mat4 transform, bool phongShade, bool wireframeOnly);
        vector<Primitive*> getObjects();
    
    private:
        bool parseLine(string line);
    
    private:
		bool phongShading;
		bool wireframeOnly;
		int vertsParsed;
		mat4 transform;
		bool transformIsIdentity;
		Mesh* mesh;
		vector<vec3> phongNormals;
        vector<Shape*> triangles;
        Material* mat;

};

#endif